package com.sasamp.cr.gui.dialogs;

import android.text.TextPaint;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.sasamp.cr.R;
import com.sasamp.cr.gui.util.Utils;

import java.util.ArrayList;

public class DialogAdapter extends RecyclerView.Adapter {
    private int mCurrentSelectedPosition = 0;
    private ImageView mCurrentSelectedView;
    private final ArrayList<TextView> mFieldHeaders;
    private final ArrayList<String> mFieldTexts;
    private final ArrayList<ArrayList<TextView>> mFields;
    private OnClickListener mOnClickListener;
    private OnDoubleClickListener mOnDoubleClickListener;

    public interface OnClickListener {
        void onClick(int i, String str);
    }

    public interface OnDoubleClickListener {
        void onDoubleClick();
    }

    public DialogAdapter(ArrayList<String> fields, ArrayList<TextView> fieldHeaders) {
        this.mFieldTexts = fields;
        this.mFieldHeaders = fieldHeaders;
        this.mFields = new ArrayList<>();
    }

    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new ViewHolder(LayoutInflater.from(parent.getContext()).inflate(R.layout.sd_dialog_item, parent, false));
    }

    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        onBindViewHolder((ViewHolder) holder, position);
    }

    public void onBindViewHolder(ViewHolder holder, int position) {
        String[] headers = this.mFieldTexts.get(position).split("\t");
        ArrayList<TextView> fields = new ArrayList<>();
        for (int i = 0; i < headers.length; i++) {
            if(i > holder.mFields.size())continue;

            TextView field = holder.mFields.get(i);
            field.setText(Utils.transfromColors(headers[i].replace("\\t", "")));
            field.setVisibility(View.VISIBLE);
            fields.add(field);
        }
        this.mFields.add(fields);
        if (this.mCurrentSelectedPosition == position) {
            ImageView imageView = holder.mFieldBg;
            this.mCurrentSelectedView = imageView;
            imageView.setVisibility(View.VISIBLE);
            imageView.setImageResource(R.drawable.dialog_item_btn);
            this.mOnClickListener.onClick(position, holder.mFields.get(0).getText().toString());
        } else {
            holder.mFieldBg.setVisibility(View.VISIBLE);
            holder.mFieldBg.setImageResource(R.drawable.dialog_item_btn_none);
            // holder.mFieldBg.setVisibility(View.GONE);
        }

        holder.getView().setOnClickListener(view -> {
            if (this.mCurrentSelectedPosition != holder.getAdapterPosition()) {
                this.mCurrentSelectedView.setImageResource(R.drawable.dialog_item_btn_none);
                this.mCurrentSelectedPosition = holder.getAdapterPosition();
                this.mCurrentSelectedView = holder.mFieldBg;
                holder.mFieldBg.setVisibility(View.VISIBLE);
                holder.mFieldBg.setImageResource(R.drawable.dialog_item_btn);
                this.mOnClickListener.onClick(holder.getAdapterPosition(), holder.mFields.get(0).getText().toString());
                return;
            }
            OnDoubleClickListener onDoubleClickListener = this.mOnDoubleClickListener;
            if (onDoubleClickListener != null) {
                onDoubleClickListener.onDoubleClick();
            }
        });
    }

    public void updateSizes() {
        int[] max = new int[4];
        for (int i = 0; i < this.mFields.size(); i++) {
            for (int j = 0; j < this.mFields.get(i).size(); j++) {
                int width = this.mFields.get(i).get(j).getWidth();
                if (max[j] < width) {
                    max[j] = width;
                }
            }
        }
        for (int i = 0; i < this.mFieldHeaders.size(); i++) {
            TextView textView = this.mFieldHeaders.get(i);
            int headerWidth = measureTextWidth(textView);

            if (max[i] < headerWidth) {
                max[i] = headerWidth;
            }
        }
        for (int i = 0; i < this.mFields.size(); i++) {
            for (int j = 0; j < this.mFields.get(i).size(); j++) {
                this.mFields.get(i).get(j).setWidth(max[j]);
            }
        }
        for (int i = 0; i < this.mFieldHeaders.size(); i++) {
            this.mFieldHeaders.get(i).setWidth(max[i]);
        }
    }

    private int measureTextWidth(TextView view) {

        String text = String.valueOf(view.getText());

        if (TextUtils.isEmpty(text)) {
            return 0;
        }/*from  w  w  w. ja  v a2 s. c o  m*/

        TextPaint paint = view.getPaint();
        int width = (int) (paint.measureText(text) * 1.5);
        return width;
    }

    public void setOnClickListener(OnClickListener onClickListener) {
        this.mOnClickListener = onClickListener;
    }

    public void setOnDoubleClickListener(OnDoubleClickListener onDoubleClickListener) {
        this.mOnDoubleClickListener = onDoubleClickListener;
    }

    public ArrayList<ArrayList<TextView>> getFields() {
        return this.mFields;
    }

    public int getItemCount() {
        return this.mFieldTexts.size();
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        public ImageView mFieldBg;
        public ArrayList<TextView> mFields = new ArrayList<>();
        private final View mView;

        public ViewHolder(View itemView) {
            super(itemView);
            this.mView = itemView;
            this.mFieldBg = (ImageView) itemView.findViewById(R.id.sd_dialog_item_bg);
            ConstraintLayout field = itemView.findViewById(R.id.sd_dialog_item_main);
            for (int i = 1; i < field.getChildCount(); i++) {
                this.mFields.add((TextView) field.getChildAt(i));
            }
        }

        public View getView() {
            return this.mView;
        }
    }
}
