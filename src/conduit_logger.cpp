#include "conduit_logger.hpp"

namespace conduit {
    auto conduit_logger::s_output_stream = std::ofstream{};
    auto conduit_logger::s_conduit_logger = conduit_logger{};
}