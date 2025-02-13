package com.sasamp.launcher.async.task;

import static com.sasamp.launcher.config.Config.FILE_INFO_URL;

import android.view.WindowManager;
import android.widget.Toast;

import androidx.annotation.UiThread;
import androidx.annotation.WorkerThread;
import androidx.fragment.app.FragmentActivity;

import com.sasamp.launcher.async.domain.AsyncTaskResult;
import com.sasamp.launcher.async.dto.response.FileInfo;
import com.sasamp.launcher.async.dto.response.GameFileInfoDto;
import com.sasamp.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.sasamp.launcher.async.listener.OnAsyncSuccessListenerWithResponse;
import com.sasamp.launcher.error.apiException.ApiException;
import com.sasamp.launcher.error.apiException.ErrorContainer;
import com.techyourchance.threadposter.BackgroundThreadPoster;
import com.techyourchance.threadposter.UiThreadPoster;

import org.apache.commons.lang3.StringUtils;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.http.converter.json.MappingJackson2HttpMessageConverter;
import org.springframework.web.client.RestTemplate;

import java.io.File;
import java.net.URI;
import java.util.Collections;
import java.util.HashSet;
import java.util.Optional;
import java.util.Set;

public class CacheChecker implements Listener<FileInfo[]> {

    private final FragmentActivity activity;

    private final UiThreadPoster uiThreadPoster;
    private final BackgroundThreadPoster backgroundThreadPoster;

    private OnAsyncSuccessListenerWithResponse<FileInfo[]> onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;

    private final static Set<String> NOT_CHECK_BY_HASH_FILES;

    public CacheChecker(FragmentActivity activity) {
        this.activity = activity;
    }
    {
        uiThreadPoster = new UiThreadPoster();
        backgroundThreadPoster = new BackgroundThreadPoster();
    }

    static {
        NOT_CHECK_BY_HASH_FILES = new HashSet<>();

        NOT_CHECK_BY_HASH_FILES.add("files/gta_sa.set");
        NOT_CHECK_BY_HASH_FILES.add("files/GTASAMP10.b");
        NOT_CHECK_BY_HASH_FILES.add("files/SAMP/settings.ini");
        NOT_CHECK_BY_HASH_FILES.add("files/gtasatelem.set");
        NOT_CHECK_BY_HASH_FILES.add("files/samp_log.txt");
        NOT_CHECK_BY_HASH_FILES.add("files/crash_log.log");
        NOT_CHECK_BY_HASH_FILES.add("files/Adjustable.cfg");
    }

    public void checkIsAllCacheFilesExist() {
        backgroundThreadPoster.post(() -> checkIsFilesExist());
    }

    public void validateCache() {
        backgroundThreadPoster.post(() -> checkFilesHash());
    }

    @WorkerThread
    private void checkFilesHash() {
        try {
            GameFileInfoDto gameFilesInfo = getGameFilesInfo();
            checkHash(gameFilesInfo);
        } catch (ApiException e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(e));
            });
        } catch (Exception e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR)));
            });
        }
    }

    @WorkerThread
    private void checkIsFilesExist() {
        try {
            GameFileInfoDto gameFilesInfo = getGameFilesInfo();
            checkIsExist(gameFilesInfo);
        } catch (ApiException e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(e));
            });
        } catch (Exception e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR)));
            });
        }
    }

    private void checkHash(GameFileInfoDto gameFileInfo) {
        FileInfo[] filesToReload = Optional.ofNullable(gameFileInfo.getFiles())
                .orElse(Collections.emptyList())
                .stream()
                .filter(fileInfo -> !NOT_CHECK_BY_HASH_FILES.contains(fileInfo.getPath()) || isNotExistFile(fileInfo))
                .filter(this::isInvalidFile)
                .peek(this::removeFile)
                .toArray(FileInfo[]::new);

        uiThreadPoster.post(() -> {
            onAsyncFinished(new AsyncTaskResult<>(filesToReload));
        });
    }

    private void checkIsExist(GameFileInfoDto gameFileInfo) {
        FileInfo[] filesToReload = Optional.ofNullable(gameFileInfo.getFiles())
                .orElse(Collections.emptyList())
                .stream()
                .filter(this::isNotExistFile)
                .peek(this::removeFile)
                .toArray(FileInfo[]::new);

        uiThreadPoster.post(() -> {
            onAsyncFinished(new AsyncTaskResult<>(filesToReload));
        });
    }

    private void removeFile(FileInfo fileInfo) {
        String filePath = fileInfo.getPath().replace("files/", "");
        File file = new File(activity.getExternalFilesDir(null).toString()
                .concat("/")
                .concat(filePath)
        );

        if (file.exists()) {
            file.delete();
        }
    }

    private boolean isNotExistFile(FileInfo fileInfo) {
        String filePath = fileInfo.getPath().replace("files/", "");
        File file = new File(activity.getExternalFilesDir(null).toString()
                .concat("/")
                .concat(filePath)
        );

        return !file.exists();
    }

    private boolean isInvalidFile(FileInfo fileInfo)
    {
        String filePath = fileInfo.getPath().replace("files/", "");
        File file = new File(activity.getExternalFilesDir(null).toString()
                .concat("/")
                .concat(filePath)
        );
        return !file.exists() || file.length() != fileInfo.getSize() || file.lastModified() < fileInfo.getVer();
    }

    private GameFileInfoDto getGameFilesInfo() {
        URI url = URI.create(FILE_INFO_URL);

        RestTemplate restTemplate = new RestTemplate();
        restTemplate.getMessageConverters().add(new MappingJackson2HttpMessageConverter());
        ResponseEntity<GameFileInfoDto> response = restTemplate.exchange(url, HttpMethod.GET, HttpEntity.EMPTY, GameFileInfoDto.class);

        return response.getBody();
    }

    private String bytesToHex(byte[] raw) {
        if (raw == null) {
            return null;
        } else {
            StringBuilder hex = new StringBuilder(2 * raw.length);
            byte[] var2 = raw;
            int var3 = raw.length;

            for(int var4 = 0; var4 < var3; ++var4) {
                byte b = var2[var4];
                hex.append(Character.forDigit((b & 240) >> 4, 16)).append(Character.forDigit(b & 15, 16));
            }

            return hex.toString();
        }
    }

    public void setOnAsyncSuccessListener(OnAsyncSuccessListenerWithResponse<FileInfo[]> onClickListener) {
        this.onAsyncSuccessListener = onClickListener;
    }

    public void setOnAsyncCriticalErrorListener(OnAsyncCriticalErrorListener onAsyncCriticalErrorListener) {
        this.onAsyncCriticalErrorListener = onAsyncCriticalErrorListener;
    }

    @UiThread
    @Override
    public void onAsyncFinished(AsyncTaskResult<FileInfo[]> result) {
        activity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);
        activity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        if (result.getException() != null) {
            ApiException apiException = result.getException();
            showErrorMessage(apiException);
            onAsyncErrorDo();
        } else if (onAsyncSuccessListener != null) {
            onAsyncSuccessListener.onSuccess(result.getResult());
        }
    }

    private void showErrorMessage(ApiException exception) {
        String errorMessage = Optional.ofNullable(exception)
                .map(ApiException::getError)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        Toast.makeText(activity, errorMessage, Toast.LENGTH_SHORT).show();
    }

    private void onAsyncErrorDo() {
        if (onAsyncCriticalErrorListener != null) {
            onAsyncCriticalErrorListener.onCriticalError();
        }
    }
}
