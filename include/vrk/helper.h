#pragma once

#define PRINT_RETURN_CODE(stream, code, functionName) \
    fprintf(stream, "Return code %d (%s)\n", code, functionName)

#define PRINT_MESSAGE(stream, message) \
    fprintf(stream, "%s\n", message)
