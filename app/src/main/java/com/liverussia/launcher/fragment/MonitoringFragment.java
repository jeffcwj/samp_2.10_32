package com.liverussia.launcher.fragment;

import android.os.Bundle;          
import android.os.Environment;
import android.os.Build;

import android.widget.*;
import android.graphics.PorterDuff;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.annotation.NonNull;
import android.content.Context;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;

import android.view.View.OnClickListener;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.view.animation.Animation;
import android.view.LayoutInflater;
import android.view.ViewGroup;

import java.util.ArrayList;
import java.util.List;

import com.liverussia.cr.R;
import com.liverussia.launcher.adapter.NewsAdapter;
import com.liverussia.launcher.model.News;
import com.liverussia.launcher.other.Lists;

import com.liverussia.launcher.adapter.ServersAdapter;
import com.liverussia.launcher.model.Servers;

import com.liverussia.launcher.activity.SplashActivity;

public class MonitoringFragment extends Fragment {
	
	RecyclerView recyclerNews;
	NewsAdapter newsAdapter;
	ArrayList<News> nlist;
	
	RecyclerView recyclerServers;
	ServersAdapter serversAdapter;
	ArrayList<Servers> slist;
	
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View inflate = inflater.inflate(R.layout.fragment_monitoring, container, false);
    	Animation animation = AnimationUtils.loadAnimation(getActivity(), R.anim.button_click);
		
	    recyclerNews = inflate.findViewById(R.id.newsRV);
		recyclerNews.setHasFixedSize(true);
		LinearLayoutManager layoutManager = new LinearLayoutManager(getActivity(), LinearLayoutManager.HORIZONTAL, false);
		recyclerNews.setLayoutManager(layoutManager);
		
		this.nlist = Lists.nlist;
		newsAdapter = new NewsAdapter(getContext(), this.nlist);
		recyclerNews.setAdapter(newsAdapter);
		
		recyclerServers = inflate.findViewById(R.id.ourServersRV);
		recyclerServers.setHasFixedSize(true);
		LinearLayoutManager layoutManagerr = new LinearLayoutManager(getActivity());
		recyclerServers.setLayoutManager(layoutManagerr);
		
		this.slist = Lists.slist;
		serversAdapter = new ServersAdapter(getContext(), this.slist);
		recyclerServers.setAdapter(serversAdapter);
		
        return inflate;
    }
}