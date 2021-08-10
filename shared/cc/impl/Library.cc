#include <iostream>
#include <jni.h>
#include "Library.hh"
#include <cassert>
#include "Key.hh"
#include "MouseButton.hh"
#include "JNILocal.hh"

namespace jwm {

    namespace classes {
        namespace Throwable {
            jmethodID kPrintStackTrace;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("java/lang/Throwable");
                kPrintStackTrace = env->GetMethodID(cls, "printStackTrace", "()V");
            }

            bool exceptionThrown(JNIEnv* env) {
                if (env->ExceptionCheck()) {
                    jwm::JNILocal<jthrowable> th(env, env->ExceptionOccurred());
                    env->CallVoidMethod(th.get(), kPrintStackTrace);
                    if (env->ExceptionCheck())
                        env->DeleteLocalRef(env->ExceptionOccurred()); // ignore
                    return true;
                } else
                    return false;
            }
        }

        namespace Consumer {
            jmethodID kAccept;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("java/util/function/Consumer");
                Throwable::exceptionThrown(env);
                kAccept = env->GetMethodID(cls, "accept", "(Ljava/lang/Object;)V");
                Throwable::exceptionThrown(env);
            }

            bool accept(JNIEnv* env, jobject consumer, jobject event) {
                env->CallVoidMethod(consumer, kAccept, event);
                return Throwable::exceptionThrown(env);
            }
        }

        namespace Runnable {
            jmethodID kRun;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("java/lang/Runnable");
                Throwable::exceptionThrown(env);
                kRun = env->GetMethodID(cls, "run", "()V");
                Throwable::exceptionThrown(env);
            }

            bool run(JNIEnv* env, jobject runnable) {
                env->CallVoidMethod(runnable, kRun);
                return Throwable::exceptionThrown(env);
            }
        }

        namespace Native {
            jfieldID kPtr;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/impl/Native");
                Throwable::exceptionThrown(env);
                kPtr = env->GetFieldID(cls, "_ptr", "J");
                Throwable::exceptionThrown(env);
            }

            uintptr_t fromJava(JNIEnv* env, jobject object) {
                jlong ptr = env->GetLongField(object, kPtr);
                return static_cast<uintptr_t>(ptr);
            }
        }

        namespace Clipboard {
            jclass kCls;
            jmethodID kRegisterPredefinedFormat;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/Clipboard");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                // public static ClipboardFormat _registerPredefinedFormat(String formatId)
                kRegisterPredefinedFormat = env->GetStaticMethodID(kCls,
                "_registerPredefinedFormat",
                "(Ljava/lang/String;)Lorg/jetbrains/jwm/ClipboardFormat;"
                );
                Throwable::exceptionThrown(env);
                assert(kRegisterPredefinedFormat);
            }

            jobject registerFormat(JNIEnv* env, jstring formatId) {
                jobject clipboardFormat = env->CallStaticObjectMethod(kCls, kRegisterPredefinedFormat, formatId);
                return Throwable::exceptionThrown(env)? nullptr: clipboardFormat;
            }
        }

        namespace ClipboardEntry {
            jclass kCls;
            jmethodID kMake;
            jfieldID kFormat;
            jfieldID kData;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/ClipboardEntry");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                // public static ClipboardEntry make(ClipboardFormat format, byte[] data)
                kMake = env->GetStaticMethodID(kCls, "make",
                "(Lorg/jetbrains/jwm/ClipboardFormat;[B)"
                    "Lorg/jetbrains/jwm/ClipboardEntry;"
                );
                Throwable::exceptionThrown(env);
                assert(kMake);

                kFormat = env->GetFieldID(kCls, "_format", "Lorg/jetbrains/jwm/ClipboardFormat;");
                Throwable::exceptionThrown(env);
                assert(kFormat);

                kData = env->GetFieldID(kCls, "_data", "[B");
                Throwable::exceptionThrown(env);
                assert(kData);
            }

            jobject make(JNIEnv* env, jobject format, jbyteArray data) {
                jobject clipboardEntry = env->CallStaticObjectMethod(kCls, kMake, format, data);
                return Throwable::exceptionThrown(env)? nullptr: clipboardEntry;
            }

            jobject getFormat(JNIEnv* env, jobject clipboardEntry) {
                jobject format = env->GetObjectField(clipboardEntry, kFormat);
                return Throwable::exceptionThrown(env)? nullptr: format;
            }

            jbyteArray getData(JNIEnv* env, jobject clipboardEntry) {
                jobject data = env->GetObjectField(clipboardEntry, kData);
                return Throwable::exceptionThrown(env)? nullptr: static_cast<jbyteArray>(data);
            }
        }

        namespace ClipboardFormat {
            jclass kCls;
            jfieldID kFormatId;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/ClipboardFormat");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                kFormatId = env->GetFieldID(cls, "_formatId", "Ljava/lang/String;");
                Throwable::exceptionThrown(env);
                assert(kFormatId);
            }

            jstring getFormatId(JNIEnv* env, jobject clipboardFormat) {
                jobject formatId = env->GetObjectField(clipboardFormat, kFormatId);
                return Throwable::exceptionThrown(env)? nullptr: static_cast<jstring>(formatId);
            }
        }

        namespace EventClose {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventClose");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lorg/jetbrains/jwm/EventClose;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventFrame {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventFrame");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lorg/jetbrains/jwm/EventFrame;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventKeyboard {
            jclass kCls;
            jmethodID kCtor;
            jobjectArray kKeys;

            void onLoad(JNIEnv* env) {
                // kCls = EventKeyboard
                {
                    jclass cls = env->FindClass("org/jetbrains/jwm/EventKeyboard");
                    Throwable::exceptionThrown(env);
                    kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                    assert(kCls);
                }

                // kCtor = EventKeyboard::<init>(Key key, boolean isPressed, int modifiers)
                {
                    kCtor = env->GetMethodID(kCls, "<init>", "(IZI)V");
                    Throwable::exceptionThrown(env);
                    assert(kCtor);
                }
            }

            jobject make(JNIEnv* env, Key keyCode, jboolean isPressed, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, static_cast<int>(keyCode), isPressed, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventMouseButton {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventMouseButton");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(IZI)V");
                assert(kCtor);
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, MouseButton mouseButton, jboolean isPressed, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, static_cast<int>(mouseButton), isPressed, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventMouseMove {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventMouseMove");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(IIII)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint x, jint y, jint buttons, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, x, y, buttons, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventReconfigure {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventReconfigure");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lorg/jetbrains/jwm/EventReconfigure;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventResize {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventResize");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(II)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint width, jint height) {
                jobject res = env->NewObject(kCls, kCtor, width, height);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }
    
        namespace EventScroll {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventScroll");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(FFI)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jfloat dx, jfloat dy, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, dx, dy, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventTextInput {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                // kCls = EventTextInput
                {
                    jclass cls = env->FindClass("org/jetbrains/jwm/EventTextInput");
                    Throwable::exceptionThrown(env);
                    kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                    assert(kCls);
                }

                // kCtor = EventTextInput::<init>(String text)
                {
                    kCtor = env->GetMethodID(kCls, "<init>", "(Ljava/lang/String;)V");
                    Throwable::exceptionThrown(env);
                    assert(kCtor);
                }
            }

            jobject make(JNIEnv* env, jstring text) {
                jobject res = env->NewObject(kCls, kCtor, text);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventWindowMove {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventWindowMove");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(II)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint left, jint top) {
                jobject res = env->NewObject(kCls, kCtor, left, top);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace Screen {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/Screen");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(JIIIIFZ)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jlong id, jint x, jint y, jint width, jint height, jfloat scale, jboolean isPrimary) {
                jobject res = env->NewObject(kCls, kCtor, id, x, y, width, height, scale, isPrimary);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_impl_Library__1nAfterLoad
  (JNIEnv* env, jclass jclass) {
    jwm::classes::Throwable::onLoad(env);
    jwm::classes::Consumer::onLoad(env);
    jwm::classes::Runnable::onLoad(env);
    jwm::classes::Native::onLoad(env);
    jwm::classes::Clipboard::onLoad(env);
    jwm::classes::ClipboardEntry::onLoad(env);
    jwm::classes::ClipboardFormat::onLoad(env);
    jwm::classes::EventClose::onLoad(env);
    jwm::classes::EventFrame::onLoad(env);
    jwm::classes::EventKeyboard::onLoad(env);
    jwm::classes::EventMouseButton::onLoad(env);
    jwm::classes::EventMouseMove::onLoad(env);
    jwm::classes::EventReconfigure::onLoad(env);
    jwm::classes::EventResize::onLoad(env);
    jwm::classes::EventScroll::onLoad(env);
    jwm::classes::EventWindowMove::onLoad(env);
    jwm::classes::EventTextInput::onLoad(env);
    jwm::classes::Screen::onLoad(env);
}
