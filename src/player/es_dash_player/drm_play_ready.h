/*!
 * drm_play_ready.h (https://github.com/SamsungDForum/NativePlayer)
 * Copyright 2016, Samsung Electronics Co., Ltd
 * Licensed under the MIT license
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @author Tomasz Borkowski
 */

#ifndef NATIVE_PLAYER_SRC_PLAYER_ES_DASH_PLAYER_DRM_PLAY_READY_H_
#define NATIVE_PLAYER_SRC_PLAYER_ES_DASH_PLAYER_DRM_PLAY_READY_H_

#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nacl_player/drm_listener.h"
#include "nacl_player/media_player.h"
#include "ppapi/cpp/instance_handle.h"
#include "ppapi/cpp/message_loop.h"
#include "ppapi/utility/completion_callback_factory.h"

#include "dash/content_protection_visitor.h"

namespace pp {
class MessageLoop;
class URLRequestInfo;
}

namespace dash {
namespace mpd {
class IDescriptor;
}
}

class DrmPlayReadyContentProtectionDescriptor
    : public ContentProtectionDescriptor {
 public:
  DrmPlayReadyContentProtectionDescriptor() : ContentProtectionDescriptor() {}
  ~DrmPlayReadyContentProtectionDescriptor() override {}

  std::string scheme_id_uri_;
  std::string system_url_;
  std::unordered_map<std::string, std::string> key_request_properties_;

  std::string init_data_type_;
  std::vector<uint8_t> init_data_;
};

/// Provided by application, opaque to dash_parser
class DrmPlayReadyContentProtectionVisitor : public ContentProtectionVisitor {
 public:
  DrmPlayReadyContentProtectionVisitor() : ContentProtectionVisitor() {}
  ~DrmPlayReadyContentProtectionVisitor() override {}

  std::shared_ptr<ContentProtectionDescriptor> Visit(
      const std::vector<dash::mpd::IDescriptor*>& cp) override;
};

class DrmPlayReadyListener : public Samsung::NaClPlayer::DRMListener {
 public:
  DrmPlayReadyListener(
      const pp::InstanceHandle& instance,
      std::shared_ptr<Samsung::NaClPlayer::MediaPlayer> player);
  ~DrmPlayReadyListener() override{};

  void OnInitdataLoaded(Samsung::NaClPlayer::DRMType drm_type,
                        uint32_t init_data_size,
                        const void* init_data) override;

  void OnLicenseRequest(uint32_t request_size, const void* request) override;

  inline void SetContentProtectionDescriptor(
      std::shared_ptr<DrmPlayReadyContentProtectionDescriptor> cp_descriptor) {
    cp_descriptor_ = cp_descriptor;
  }

  bool IsInitialized() const;
  void Reset();

 private:
  void ProcessLicenseRequestOnSideThread(int32_t,
                                         const std::string& url,
                                         pp::URLRequestInfo lic_request);

  pp::InstanceHandle instance_;
  pp::MessageLoop side_thread_loop_;
  pp::CompletionCallbackFactory<DrmPlayReadyListener> cc_factory_;
  std::shared_ptr<DrmPlayReadyContentProtectionDescriptor> cp_descriptor_;
  std::shared_ptr<Samsung::NaClPlayer::MediaPlayer> player_;
  std::atomic<int> pending_licence_requests_;
};

#endif  // NATIVE_PLAYER_SRC_PLAYER_ES_DASH_PLAYER_DRM_PLAY_READY_H_
