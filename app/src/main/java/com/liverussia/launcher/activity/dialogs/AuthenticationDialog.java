package com.liverussia.launcher.activity.dialogs;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.TextView;

import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;

import com.google.android.gms.safetynet.SafetyNet;
import com.google.android.gms.safetynet.SafetyNetApi;
import com.liverussia.cr.R;
import com.liverussia.launcher.activity.ActivitySupportedServerSelection;
import com.liverussia.launcher.activity.ForumActivity;
import com.liverussia.launcher.config.Config;
import com.liverussia.launcher.dto.request.LoginRequestDto;
import com.liverussia.launcher.enums.BillingParameters;
import com.liverussia.launcher.enums.ServerInfo;
import com.liverussia.launcher.messages.ErrorMessages;
import com.liverussia.launcher.async.AuthenticateAsyncRestCall;
import com.liverussia.launcher.model.Servers;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.utils.Validator;

import org.jetbrains.annotations.NotNull;

import lombok.Getter;

public class AuthenticationDialog extends DialogFragment implements View.OnClickListener,
        ActivitySupportedServerSelection {

    public static final String TAG = "authentication_dialog";

    private Animation animation;

    private ActivityService activityService;

    private Servers selectedServer;
    private String captchaToken;

    private EditText passwordInput;
    private EditText loginInput;

    private TextView goBackButton;
    private TextView selectServerButton;
    private TextView loginButton;
    private CheckBox iAmNotRobotCheckBox;

    @Getter
    private Fragment fragment;

    //TODO FragmentManager
    public static AuthenticationDialog display(Fragment fragment, ActivityService activityService) {
        AuthenticationDialog dialog = new AuthenticationDialog(fragment, activityService);
        dialog.show(fragment.getFragmentManager(), TAG);
        return dialog;
    }

    private AuthenticationDialog(Fragment fragment, ActivityService activityService) {
        super();
        this.fragment = fragment;
        this.activityService = activityService;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(DialogFragment.STYLE_NORMAL, R.style.AppTheme_FullScreenDialog);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        View view = inflater.inflate(R.layout.auth_dialog, container, false);
        getDialog().getWindow().setBackgroundDrawable(getResources().getDrawable(R.drawable.auth_choose_serv_bg));

        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);

        passwordInput = view.findViewById(R.id.password_input);
        loginInput = view.findViewById(R.id.nick_input);

        goBackButton = view.findViewById(R.id.btnGoBack);
        goBackButton.setOnClickListener(this);

        selectServerButton = view.findViewById(R.id.btnSelectServer);
        selectServerButton.setOnClickListener(this);

        loginButton = view.findViewById(R.id.btnLogin);
        loginButton.setOnClickListener(this);

        iAmNotRobotCheckBox = view.findViewById(R.id.i_am_not_robot_checkbox);
        iAmNotRobotCheckBox.setOnClickListener(this);

        return view;
    }

    @Override
    public void onViewCreated(@NotNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnLogin:
                v.startAnimation(animation);
                performLoginButtonAction();
                break;
            case R.id.i_am_not_robot_checkbox:

                try {
                    performIAmNotRobotCheckBoxAction();
                } catch (Exception exception) {
                    activityService.showBigMessage(exception.getMessage(), fragment.getActivity());
                }
                break;
            case R.id.btnSelectServer:
                v.startAnimation(animation);
                performSelectServerButtonAction();
                break;
            case R.id.btnGoBack:
                v.startAnimation(animation);
                dismiss();
                break;
            default:
                break;
        }
    }

    @Override
    public void onDismiss(@NotNull DialogInterface dialog) {
        super.onDismiss(dialog);

        if (fragment instanceof DialogInterface.OnDismissListener) {
            ((DialogInterface.OnDismissListener) fragment).onDismiss(dialog);
        }
    }

    @Override
    public void setSelectedServer(Servers selectedServer) {
        this.selectedServer = selectedServer;
        selectServerButton.setCompoundDrawables(null, null, null, null);
        selectServerButton.setText(selectedServer.getname().toUpperCase());
    }

    private void performIAmNotRobotCheckBoxAction() {
        if (iAmNotRobotCheckBox.isChecked()) {
            iAmNotRobotCheckBox.setChecked(false);


            new ReCaptchaDialog(this);


//            AlertDialog.Builder alert = new AlertDialog.Builder(fragment.getActivity());
//
//            WebView webView = new WebView(fragment.getActivity());
//
//            webView.getSettings().setJavaScriptEnabled(true);
//            webView.getSettings().setDomStorageEnabled(true);
//            webView.setScrollBarStyle(0);
//            webView.getSettings().setUseWideViewPort(false);
//            webView.getSettings().setLoadWithOverviewMode(true);
//            webView.loadUrl(Config.URL_RE_CAPTCHA);
//
//            webView.setWebViewClient(new WebViewClient()
//            {
//                public boolean shouldOverrideUrlLoading(WebView view, String url)
//                {
//                    view.loadUrl(url);
//
//                    return true;
//                }
//            });
//
//            alert.setView(webView);
//
//            alert.show();


        } else {
            activityService.showMessage(ErrorMessages.CAPTCHA_NOT_PASSED.getText(), fragment.getActivity());
        }
    }

    private void performCaptchaFailureAction(Exception exception) {
        activityService.showMessage(ErrorMessages.CAPTCHA_COMPLETING_PROBLEMS.getText(), fragment.getActivity());
        iAmNotRobotCheckBox.setChecked(false);

        Log.d("Captcha failed reason: ", exception.getMessage());
    }

    private void performCaptchaSuccessAction(SafetyNetApi.RecaptchaTokenResponse recaptchaTokenResponse) {
        captchaToken = recaptchaTokenResponse.getTokenResult();
        iAmNotRobotCheckBox.setChecked(true);
    }

    private void performLoginButtonAction() {
        if (!isValidInputs()) {
            return;
        }

        authenticateUser();
    }

    private void authenticateUser() {
        LoginRequestDto requestDto = new LoginRequestDto();
        requestDto.setLogin(loginInput.getText().toString());
        requestDto.setPassword(passwordInput.getText().toString());
        requestDto.setServerId(selectedServer.getServerID());
        requestDto.setCaptchaToken(captchaToken);

        AuthenticateAsyncRestCall asyncRestCall = new AuthenticateAsyncRestCall(fragment.getActivity());
        asyncRestCall.setOnSuccessListener(this::dismiss);
        asyncRestCall.setOnAsyncCriticalErrorListener(this::performLoginFailedAction);
        asyncRestCall.loginUser(requestDto);
    }

    private void performLoginFailedAction() {
        iAmNotRobotCheckBox.setChecked(false);
        captchaToken = null;
    }

    private boolean isValidInputs() {
        return Validator.isValidNickname(loginInput.getText().toString(), fragment.getActivity())
                && Validator.isValidPassword(passwordInput.getText().toString(), fragment.getActivity())
                && Validator.isValidSelectedServer(selectedServer, fragment.getActivity())
                && Validator.isValidCaptchaToken(captchaToken, fragment.getActivity());
    }

    //TODO FragmentManager
    private void performSelectServerButtonAction() {
        SelectServerDialog.display(fragment.getFragmentManager(), this);
    }
}
