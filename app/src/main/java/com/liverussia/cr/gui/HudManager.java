package com.liverussia.cr.gui;

import android.app.Activity;
import android.text.Html;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.nvidia.devtech.NvEventQueueActivity;
import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.ArrayList;
import java.util.Formatter;

public class HudManager {
    private int hideCount;
    private Activity activity;
    private ConstraintLayout hud_main;
    private ConstraintLayout hud_main_layout;
    private ConstraintLayout target_notify;
    private ConstraintLayout yearn_money;
    private ConstraintLayout bus_layout;
    private TextView bus_text;
    private TextView yearn_moneytext;
    private ImageView target_notify_exit;
    private ImageView target_notify_status;
    private TextView target_notify_text;
    private TextView hud_money;
    private ImageView hud_weapon;
    private ImageView hud_menu;
    private TextView hud_ammo;
    private TextView levelinfo;
    private ImageView hud_greenzone;
    private ImageView hud_gpsactive;
    private ImageView hud_serverlogo;
    private ConstraintLayout hud_serverinfo;
    private ArrayList<ImageView> hud_wanted;
    private ProgressBar progressHP;
    private ProgressBar progressArmor;
    private ProgressBar oil_water_progress;
    private ProgressBar oil_oil_progress;
    private TextView oil_water_procent;
    private TextView oil_oil_procent;

    public HudManager(Activity aactivity) {
        activity = aactivity;
        hud_main = aactivity.findViewById(R.id.hud_main);
        hud_main.setVisibility(View.GONE);

        hud_main_layout = aactivity.findViewById(R.id.hud_layout);
        hud_main_layout.setVisibility(View.VISIBLE);

        bus_layout = aactivity.findViewById(R.id.bus_layout);
        bus_layout.setVisibility(View.GONE);

        bus_text = aactivity.findViewById(R.id.bus_text);

        target_notify = aactivity.findViewById(R.id.target_notify);
        target_notify.setVisibility(View.GONE);

        yearn_money = aactivity.findViewById(R.id.yearn_money);
        yearn_money.setVisibility(View.GONE);

        yearn_moneytext = aactivity.findViewById(R.id.yearn_moneytext);
        target_notify_exit = aactivity.findViewById(R.id.target_notify_exit);
        target_notify_status = aactivity.findViewById(R.id.target_notify_status);
        target_notify_text = aactivity.findViewById(R.id.target_notify_text);

        hud_greenzone = aactivity.findViewById(R.id.hud_greenzone);
        hud_gpsactive = aactivity.findViewById(R.id.hud_gpsactive);
        hud_serverlogo = aactivity.findViewById(R.id.hud_logo);
        hud_serverlogo.setVisibility(View.GONE);
        hud_serverinfo = aactivity.findViewById(R.id.hud_serverinfo);

        levelinfo = aactivity.findViewById(R.id.levelinfo);

        hud_ammo = aactivity.findViewById(R.id.hud_ammo);

        progressArmor = aactivity.findViewById(R.id.hud_armor_pb);
        progressHP = aactivity.findViewById(R.id.hud_health_pb);

        hud_money = aactivity.findViewById(R.id.hud_money);
        hud_weapon = aactivity.findViewById(R.id.hud_weapon);
        hud_menu = aactivity.findViewById(R.id.hud_menu);

        hud_wanted = new ArrayList<>();
        hud_wanted.add(activity.findViewById(R.id.hud_star_1));
        hud_wanted.add(activity.findViewById(R.id.hud_star_2));
        hud_wanted.add(activity.findViewById(R.id.hud_star_3));
        hud_wanted.add(activity.findViewById(R.id.hud_star_4));
        hud_wanted.add(activity.findViewById(R.id.hud_star_5));
        hud_wanted.add(activity.findViewById(R.id.hud_star_6));
        hud_menu.setOnClickListener( view -> {
            NvEventQueueActivity.getInstance().showMenu();
            NvEventQueueActivity.getInstance().togglePlayer(1);
        });

        oil_water_progress = aactivity.findViewById(R.id.oil_water_progress);
        oil_oil_progress = aactivity.findViewById(R.id.oil_oil_progress);
        oil_water_procent = aactivity.findViewById(R.id.oil_water_procent);
        oil_oil_procent = aactivity.findViewById(R.id.oil_oil_procent);

        Utils.HideLayout(hud_gpsactive, false);
    }

    public void UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoclip, int money, int wanted)
    {
        progressHP.setProgress(health);
        progressArmor.setProgress(armour);

        DecimalFormat formatter=new DecimalFormat();
        DecimalFormatSymbols symbols= DecimalFormatSymbols.getInstance();
        symbols.setGroupingSeparator('.');
        formatter.setDecimalFormatSymbols(symbols);
        String s=formatter.format(money).toString();
        hud_money.setText(String.valueOf(s));

        int id = activity.getResources().getIdentifier(new Formatter().format("weapon_%d", Integer.valueOf(weaponid)).toString(), "drawable", activity.getPackageName());
        hud_weapon.setImageResource(id);

        hud_weapon.setOnClickListener(v -> NvEventQueueActivity.getInstance().onWeaponChanged());
        if(wanted > 6) wanted = 6;
        for (int i2 = 0; i2 < wanted; i2++) {
            hud_wanted.get(i2).setBackgroundResource(R.drawable.ic_y_star);
        }

        if (weaponid > 15 & weaponid < 44 & weaponid != 21)
        {
            Utils.ShowLayout(hud_ammo, false);
            String ss = String.format("%d<font color='#B0B0B0'>/%d</font>", ammoclip, ammo - ammoclip);
            hud_ammo.setText(Html.fromHtml(String.valueOf(ss)));
        }
        else
        {
            Utils.HideLayout(hud_ammo, false);
        }

        String stroilwaterproc = String.format("%d%%", oil_water_progress.getProgress()/10);
        oil_water_procent.setText(String.valueOf(stroilwaterproc));
        String stroiloilproc = String.format("%d%%", oil_oil_progress.getProgress()/10);
        oil_oil_procent.setText(String.valueOf(stroiloilproc));

    }

    public void ShowHud()
    {
        hideCount --;
        if(hideCount <= 0)
        {
            hideCount = 0;
            Utils.ShowLayout(hud_main, false);
            NvEventQueueActivity.getInstance().ToggleHud(true, true);
        }
    }

    public void HideHud(boolean withChar)
    {
        Log.d("sdfs", "HIDE!!!!!");
        hideCount ++;
        Utils.HideLayout(hud_main, false);
        NvEventQueueActivity.getInstance().ToggleHud(false, withChar);
    }

    public void ShowGreenZone() { Utils.ShowLayout(hud_greenzone, true); }

    public void HideGreenZone()
    {
        Utils.HideLayout(hud_greenzone, true);
    }

    public void ShowGPS() { Utils.ShowLayout(hud_gpsactive, true); }

    public void HideGPS()
    {
        Utils.HideLayout(hud_gpsactive, true);
    }

    public void ShowServer(int serverid) {
        if (serverid == 0)
        {
            hud_serverlogo.setImageResource(R.drawable.moscow);
        }
        else if (serverid == 1)
        {
            hud_serverlogo.setImageResource(R.drawable.peterburg);
        }
        else if (serverid == 2)
        {
            hud_serverlogo.setImageResource(R.drawable.novosibirsk);
        }
        else if (serverid == 3)
        {
            hud_serverlogo.setImageResource(R.drawable.samara);
        }
        else if (serverid == 4)
        {
            hud_serverlogo.setImageResource(R.drawable.sochi);
        }
        else if (serverid == 5)
        {
            hud_serverlogo.setImageResource(R.drawable.testserver);
        }
        Utils.ShowLayout(hud_serverinfo, true);
    }
    public void HideServerLogo()
    {
        Utils.HideLayout(hud_serverlogo, false);
    }
    public void ShowServerLogo()
    {
        Utils.ShowLayout(hud_serverlogo, false);
    }
    public void HideServer() { Utils.HideLayout(hud_serverinfo, true); }

    public void UpdateLevelInfo(int level, int currentexp, int maxexp) {
        String strlevelinfo = String.format("LVL %d [EXP %d/%d]", level, currentexp, maxexp);
        levelinfo.setText(String.valueOf(strlevelinfo));
    }

    public void ShowYernMoney() { Utils.ShowLayout(yearn_money, true); }

    public void HideYernMoney() { Utils.HideLayout(yearn_money, true); }

    public void UpdateYearnMoney(int money) {
        if (money != -1)
        {
            String str = String.format("%d", money);
            yearn_moneytext.setText(String.valueOf(str) + " P");
        }
    }

    public void ShowUpdateTargetNotify(int type, String text) {
        Utils.ShowLayout(target_notify, true);
        if (type == 0)
        {
            target_notify_text.setText(text);
            target_notify_text.setTextColor(0xFFFFFFFF);
            target_notify_status.setImageResource(R.drawable.target_notify_none);
        }
        else if (type == 1)
        {
            target_notify_text.setText(text);
            target_notify_text.setTextColor(0xFF33C44F);
            target_notify_status.setImageResource(R.drawable.target_notify_success);
        }
        target_notify_exit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                NvEventQueueActivity.getInstance().onTargetNotifyClose();
            }
        });
    }

    public void HideTargetNotify() {
        Utils.HideLayout(target_notify, true);
    }

    public void ShowBusInfo(int time) {
        String strbustime = String.format("%d", time);
        bus_text.setText(String.valueOf(strbustime));
        Utils.ShowLayout(bus_layout, true);
    }

    public void HideBusInfo() { Utils.HideLayout(bus_layout, true); }


}