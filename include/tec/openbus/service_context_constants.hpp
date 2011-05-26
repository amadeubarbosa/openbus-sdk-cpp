#ifndef TEC_OPENBUS_SERVICE_CONTEXT_CONSTANTS_HPP
#define TEC_OPENBUS_SERVICE_CONTEXT_CONSTANTS_HPP

#include <boost/integer.hpp>

namespace tec { namespace openbus {

typedef boost::uint_t<32>::least context_id_type;
const context_id_type security_session_id = 0x00110000; // 0x001100(4352) is the Openbus VCSID
                                                        // 0x00 is the id of the security_session

} }

#endif
