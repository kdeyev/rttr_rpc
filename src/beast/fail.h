#pragma once

#include <boost/system/error_code.hpp>

#include "rttr_rpc_beast_export.h"

// Report a failure
void RTTR_RPC_BEAST_EXPORT fail(boost::system::error_code ec, char const* what);
