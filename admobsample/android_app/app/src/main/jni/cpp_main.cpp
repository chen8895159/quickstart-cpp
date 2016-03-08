

#include "cpp_main.h"
#include <android/log.h>
#include <cstddef>
#include "ImageData.h"

#include "../../../../../../admobcpp/include/admob.h"
#include "../../../../../../admobcpp/include/banner_view.h"

CPPMain::CPPMain() {}

static const GLchar* kVertexShaderCodeString =
    "attribute vec2 aPosition;\n"
    "attribute vec2 aTexCoord;\n"
    "varying vec2 vTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vTexCoord = aTexCoord;\n"
    "    gl_Position = vec4(aPosition, 0.0, 1.0);\n"
    "}";

static const GLchar* kFragmentShaderCodeString =
    "varying mediump vec2 vTexCoord;\n"
    "uniform vec4 color;\n"
    "uniform sampler2D texture_unit_0;\n"
    "void main() { \n"
    "  lowp vec4 texture_color = texture2D(texture_unit_0, vTexCoord);\n"
    "  gl_FragColor = texture_color;\n"
    "}";

#ifdef __APPLE__
void CPPMain::Initialize() { CPPMain::Initialize(); }
#else
void CPPMain::Initialize(JNIEnv* env, jobject activity) {
  firebase::admob::Initialize(env, activity);
  firebase::admob::AdSize adSize;
  banner_view_ = new firebase::admob::BannerView(
      activity, "ca-app-pub-3940256099942544/6300978111", adSize);
  interstitial_ad_ = new firebase::admob::InterstitialAd(
      activity, "ca-app-pub-3940256099942544/1033173712");
}
#endif
firebase::admob::AdRequest CPPMain::createRequest() {
  firebase::admob::AdRequest request;
  request.gender = firebase::admob::kGenderFemale;
  request.tagged_for_child_directed_treatment =
      firebase::admob::kChildDirectedTreatmentStateTagged;
  request.birthday_day = 10;
  request.birthday_month = 11;
  request.birthday_year = 1976;

  request.keyword_count = 3;
  request.keywords = new const char*[3];
  request.keywords[0] = "keyword1";
  request.keywords[1] = "keyword2";
  request.keywords[2] = "keyword3";

  request.extras_count = 1;
  request.extras = new firebase::admob::KeyValuePair*[1];
  request.extras[0] = new firebase::admob::KeyValuePair();
  request.extras[0]->key = "key1";
  request.extras[0]->key = "value1";

  request.test_device_id_count = 2;
  request.test_device_ids = new const char*[2];
  request.test_device_ids[0] = "YOUR_DEVICE_HASH";
  request.test_device_ids[1] = "ANOTHER_DEVICE_HASH";

  return request;
}

void CPPMain::onSurfaceCreated() {
  vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vertex_shader_, 1, &kVertexShaderCodeString, NULL);
  glCompileShader(vertex_shader_);

  GLint status;
  glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);

  __android_log_print(ANDROID_LOG_INFO, "GMACPP",
                      "vertex compilcation status: %d", status);

  char buffer[512];
  glGetShaderInfoLog(vertex_shader_, 512, NULL, buffer);

  __android_log_print(ANDROID_LOG_INFO, "GMACPP", buffer);

  glShaderSource(fragment_shader_, 1, &kFragmentShaderCodeString, NULL);
  glCompileShader(fragment_shader_);

  glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &status);

  __android_log_print(ANDROID_LOG_INFO, "GMACPP",
                      "fragment compilcation status: %d", status);

  glGetShaderInfoLog(fragment_shader_, 512, NULL, buffer);

  __android_log_print(ANDROID_LOG_INFO, "GMACPP", buffer);

  shader_program_ = glCreateProgram();
  glAttachShader(shader_program_, vertex_shader_);
  glAttachShader(shader_program_, fragment_shader_);

  glBindAttribLocation(shader_program_, 0, "aPosition");
  glBindAttribLocation(shader_program_, 1, "aTexCoord");

  glLinkProgram(shader_program_);
  glUseProgram(shader_program_);

  button_list_[0].GenerateTexture(ImageData::download_ad_);
  button_list_[1].GenerateTexture(ImageData::display_ad_);
  button_list_[2].GenerateTexture(ImageData::download_i_ad_);
  button_list_[3].GenerateTexture(ImageData::display_i_ad_);
}

void CPPMain::onSurfaceChanged(int width, int height) {
  __android_log_print(ANDROID_LOG_INFO, "GMACPP", "got dimensions: %d, %d",
                      width, height);
  width_ = width;
  height_ = height;

  GLfloat heightIncrement = 400.0f / height_;
  GLfloat currentHeight = 0.93f;

  for (int i = 0; i < 4; i++) {
    button_list_[i].SetLocation(0.0, 0.75 - 0.4 * i, 1.5, 0.3);
  }
}

void CPPMain::onDrawFrame() {
  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  firebase::admob::BoundingBox box = banner_view_->GetBoundingBox();

  for (int i = 0; i < kNumberOfButtons; i++) {
    button_list_[i].Draw(shader_program_);
  }
}

void CPPMain::onUpdate() {}

void CPPMain::onTap(float x, float y) {
  int button_number = -1;
  GLfloat viewport_x = 1 - (((width_ - x) * 2) / width_);
  GLfloat viewport_y = 1 - (((y)*2) / height_);

  for (int i = 0; i < kNumberOfButtons; i++) {
    if (button_list_[i].CheckClick(viewport_x, viewport_y)) {
      button_list_[i].SetColor(1.0f, 0.0f, 1.0f);
      button_number = i;
    }
  }

  __android_log_print(ANDROID_LOG_INFO, "GMACPP",
                      "Tap at %.2f, %.2f (%.2f, %.2f), Button #%d", x, y,
                      viewport_x, viewport_y, button_number);

  firebase::admob::BannerViewLifecycleState bannerLifecycleState =
      banner_view_->GetLifecycleState();
  firebase::admob::BannerViewPresentationState bannerPresentationState =
      banner_view_->GetPresentationState();

  firebase::admob::InterstitialAdLifecycleState interstitialLifecycleState =
      interstitial_ad_->GetLifecycleState();
  firebase::admob::InterstitialAdPresentationState
      interstitialPresentationState = interstitial_ad_->GetPresentationState();

  switch (button_number) {
    case 0:
      // Download an ad.
      if (bannerLifecycleState == firebase::admob::kBannerViewInitialized ||
          bannerLifecycleState ==
              firebase::admob::kBannerViewFailedInternalError ||
          bannerLifecycleState ==
              firebase::admob::kBannerViewFailedInvalidRequest ||
          bannerLifecycleState ==
              firebase::admob::kBannerViewFailedNetworkError ||
          bannerLifecycleState == firebase::admob::kBannerViewFailedNoFill ||
          bannerLifecycleState == firebase::admob::kBannerViewLoaded) {
        banner_view_->LoadAd(createRequest());
        __android_log_print(ANDROID_LOG_INFO, "GMACPP", "Loading ad!");
      } else {
        __android_log_print(ANDROID_LOG_INFO, "GMACPP", "no ad loading!");
      }
      break;
    case 1:
      // Display ad
      if (bannerPresentationState == firebase::admob::kBannerViewHidden) {
        banner_view_->Show();
        __android_log_print(ANDROID_LOG_INFO, "GMACPP", "showing ad!");
      } else {
        banner_view_->Hide();
        __android_log_print(ANDROID_LOG_INFO, "GMACPP", "hiding ad!");
      }
      break;
    case 2:
      // Download interstitial ad:
      interstitial_ad_->LoadAd(createRequest());
    case 3:
      // Display interstitial ad:
      interstitial_ad_->Show();
      break;
      break;
    default:
      break;
  }
}

void CPPMain::Pause() { banner_view_->Pause(); }

void CPPMain::Resume() { banner_view_->Resume(); }
