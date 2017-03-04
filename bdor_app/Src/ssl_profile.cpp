/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
 28-32 boulevard de Grenelle 75015 Paris, France.
 All rights reserved.
 This source program is the property of the INGENICO Company mentioned above
 and may not be copied in any form or by any means, whether in part or in whole,
 except under license expressly granted by such INGENICO company.
 All copies of this source program, whether in part or in whole, and
 whether modified or not, must display this and all other
 embedded copyright and ownership notices in full.
 ------------------------------------------------------------------------------
 */
#include <app_ssl_profile.h>
#include <string>
#include <utils/logger.h>

// Telium SDK
#include <sdk.h>
#include <ssl_.h>

namespace bdor {

bool LoadSingleProfile(const std::string& profile_name,
                       const std::string& ca_path,
                       int protocol,
                       long int cipher,
                       long int export_mask) {
  int ret = ssllib_open();
  if (ret != 0) {
    logger::error("FDMS - Unable to load SSL library");
    return false;
  }

  SSL_DeleteProfile(profile_name.c_str());

  // Create NEWPROFILE
  int err;
  SSL_PROFILE_HANDLE profile_handle = SSL_NewProfile(profile_name.c_str(),
                                                     &err);
  if (profile_handle == NULL) {
    logger::error("FDMS - Unable to create new SSL profile: %s",
                  profile_name.c_str());
    ssllib_close();
    return false;
  }

  // Set profile settings
  SSL_ProfileSetProtocol(profile_handle, protocol);
  SSL_ProfileSetCipher(profile_handle,
                       cipher,
                       export_mask);
  // This is the CA server certificate we loaded in the HOST
  SSL_ProfileAddCertificateCA(profile_handle, ca_path.c_str());

  /* Save the profile.*/
  SSL_SaveProfile(profile_handle);

  /* Tests whether the files linked to the profile are loaded into the terminal.*/
  SSL_HANDLE ssl_handle;
  err = SSL_New(&ssl_handle, profile_handle);
  if (err != 0) {
    SSL_UnloadProfile(profile_handle);
    SSL_DeleteProfile(profile_name.c_str());
    ssllib_close();
    logger::error("FDMS - Error when creating new profile: %s",
                  profile_name.c_str());
    return false;;
  }
  SSL_Free(ssl_handle);

  SSL_UnloadProfile(profile_handle);
  ssllib_close();
  return true;
}

bool LoadSslProfiles() {
  const std::string kFdmsProfileName = "FDMS";
  const std::string kFdmsCaPath = "/HOST/FDMSCA.CRT";

  const std::string kAmexProfileName = "AMEX";
  const std::string kAmexCaPath = "/HOST/AMEXCA.CRT";

  bool ret = LoadSingleProfile(
      kFdmsProfileName,
      kFdmsCaPath,
      TLSv1_2,
      SSL_RSA | SSL_DSS | SSL_DES | SSL_3DES | SSL_AES | SSL_RC4 | SSL_RC2 |
      SSL_SHA1 | SSL_SHA256 | SSL_SHA384,
      SSL_HIGH | SSL_NOT_EXP);

  ret &= LoadSingleProfile(
      kAmexProfileName,
      kAmexCaPath,
      TLSv1_2,
      SSL_RSA | SSL_DSS | SSL_DES | SSL_3DES | SSL_AES | SSL_RC4 | SSL_RC2 |
      SSL_SHA1 | SSL_SHA256 | SSL_SHA384,
      SSL_HIGH | SSL_NOT_EXP);

  return ret;
}

}

