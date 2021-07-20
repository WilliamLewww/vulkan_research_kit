#pragma once

#define PRINT_RETURN_CODE(stream, code, functionName) \
    fprintf(stream, "Return code %d (%s)\n", code, functionName)
