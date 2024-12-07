#pragma once

#include <stdint.h>
#include "portSupervisor.h"
#include "non_volatile_storage.h"
#include "esp_log.h"

namespace Vault {
    
    enum Result {
        OK,
        ERR,
        ERR_ALLOCATION,
        ERR_OBJ_INSTANCE,
        ERR_VAULT_NOT_INITIALIZED,
        ERR_WRITE_NUMBER_OF_PORTS,
        ERR_WRITE_CONFIG,
        ERR_READ
    };

    enum Options {
        READ,
        STORE
    };
    
    Vault::Result getVaultData(PortSupervisor::Supervisor& portManager);
    Vault::Result setVaultData(const PortSupervisor::Supervisor& portManager);
}
