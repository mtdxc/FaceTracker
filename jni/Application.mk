APP_PLATFORM := android-19
APP_STL := gnustl_static
# APP_STL := stlport_shared

APP_CPPFLAGS := -frtti -fexceptions -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
APP_LDFLAGS := -latomic

