// Copyright 2016 The Cobalt Authors. All Rights Reserved.
// Copyright 2025 Reis Can (reisxd). All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cobalt/h5vcc/h5vcc_tizen_tube.h"

#include <string.h>

#include <memory>
#include <utility>

#if defined(ANDROID)
#include "starboard/android/shared/jni_env_ext.h"
#include "starboard/android/shared/jni_utils.h"
#endif
namespace cobalt {
namespace h5vcc {

#if defined(ANDROID)
using starboard::android::shared::JniEnvExt;
using starboard::android::shared::ScopedLocalJavaRef;
#endif


H5vccTizenTube::H5vccTizenTube(
    cobalt::network::NetworkModule* network_module,
    cobalt::persistent_storage::PersistentSettings* persistent_settings)
    : network_module_{network_module},
      persistent_settings_{persistent_settings} {}

bool H5vccTizenTube::InstallAppFromURL(const std::string& url) const {
#if defined(ANDROID)
  JniEnvExt* env = JniEnvExt::Get();
  ScopedLocalJavaRef<jstring> j_url(
      env->NewStringStandardUTFOrAbort(url.c_str()));
  env->CallStarboardBooleanMethodOrAbort("installAppFromURL",
                                         "(Ljava/lang/String;)Z", j_url.Get());
  return true;
#endif
  return false;
}

std::string H5vccTizenTube::GetVersion() const {
#if defined(ANDROID)
  JniEnvExt* env = JniEnvExt::Get();
  ScopedLocalJavaRef<jstring> j_version(env->CallStarboardObjectMethodOrAbort(
      "getVersion", "()Ljava/lang/String;"));
  const char* version_cstr = env->GetStringUTFChars(j_version.Get(), nullptr);
  std::string version(version_cstr);
  env->ReleaseStringUTFChars(j_version.Get(), version_cstr);
  return version;
#endif
  return "";
}

bool H5vccTizenTube::SetUserAgent(const std::string& user_agent) const {
  persistent_settings_->Set("userAgent", base::Value(user_agent));
  network_module_->task_runner()->PostTask(
      FROM_HERE, base::Bind(
                     [](cobalt::network::NetworkModule* network_module,
                        const std::string& user_agent) {
                       auto http_user_agent_settings =
                           std::make_unique<net::StaticHttpUserAgentSettings>(
                               network_module->options_.preferred_language,
                               user_agent);
                       network_module->url_request_context_
                           ->url_request_context_->set_http_user_agent_settings(
                               std::move(http_user_agent_settings));
                     },
                     network_module_, user_agent));
  return true;
}

std::string H5vccTizenTube::GetArchitecture() const {
#if defined(ANDROID)
  JniEnvExt* env = JniEnvExt::Get();
  ScopedLocalJavaRef<jstring> j_architecture(
      env->CallStarboardObjectMethodOrAbort("getArchitecture",
                                            "()Ljava/lang/String;"));
  const char* architecture_cstr =
      env->GetStringUTFChars(j_architecture.Get(), nullptr);
  std::string architecture(architecture_cstr);
  env->ReleaseStringUTFChars(j_architecture.Get(), architecture_cstr);
  return architecture;
#endif
  return "";
}

void H5vccTizenTube::SetFrameRate(float frame_rate) const {
#if defined(ANDROID)
  JniEnvExt* env = JniEnvExt::Get();
  env->CallStarboardVoidMethodOrAbort("setFrameRate", "(F)V", frame_rate);
#endif
}

}  // namespace h5vcc
}  // namespace cobalt
