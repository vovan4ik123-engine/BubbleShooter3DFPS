package managers;

import org.libsdl.app.SDLActivity;

public class AdsManager {

    public static void init(SDLActivity activ)  {

    }

    // Called from C++ code.
    public static void showRewardedAd(boolean callbackAtCloseWindow) {

    }

    public static void showInterstitialAd() {

    }

//        AdsManager.m_activity.runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//
//            }
//        });

    public static native void rewardedAdSuccessCallback();
    public static native void rewardedAdErrorCallback();

    public static native void interstitialAdSuccessCallback();
    public static native void interstitialAdErrorCallback();
}
