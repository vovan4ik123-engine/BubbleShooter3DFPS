package managers;

import org.libsdl.app.SDLActivity;

public class BillingManager {
    
    public static void init(SDLActivity activity) {

    }

    // Called from C++ code.
    public static void makeInAppPurchase(String productID) {

    }

    public static native void billingSystemSuccessCallback();
    public static native void billingSystemErrorCallback();
}
