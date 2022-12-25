CPP := g++
MOC := moc
EXECUTABLE := daemon
CFLAGS := -std=c++23 -Wall -pedantic -Wno-deprecated-enum-enum-conversion
INCDIRS := $(shell pkg-config --cflags Qt5UiTools Qt6Widgets)
LIBS := $(shell pkg-config --libs Qt5UiTools Qt6Widgets)
MOC_HEADERS := src/main_window.h src/worker.h
SOURCES := src/main.cpp src/main_window.cpp src/worker.cpp

# Change postfixes
MOC_SOURCES := $(MOC_HEADERS:.h=.moc.cpp)
OBJECTS := $(SOURCES:.cpp=.o) $(MOC_SOURCES:.cpp=.o)

# install paths
APP_INSTALL_PATH := /usr/local/bin/joystick_virt_kbd
SERVICE_INSTALL_PATH := /etc/systemd/system
PRIVATE_DATA_PATH := /var/lib/joystick_virt_kbd

.PHONY: clean install uninstall

all: $(EXECUTABLE)

install: $(EXECUTABLE)
	sudo cp $(EXECUTABLE) $(APP_INSTALL_PATH)
	sudo cp systemd/joystick_virt_kbd.service $(SERVICE_INSTALL_PATH)/joystick_virt_kbd.service
	sudo mkdir -p $(PRIVATE_DATA_PATH)
	sudo -E sh -c 'env > $(PRIVATE_DATA_PATH)/.env'
	sudo systemctl daemon-reload
	sudo systemctl start joystick_virt_kbd

uninstall:
	sudo systemctl stop joystick_virt_kbd
	sudo rm -rf $(SERVICE_INSTALL_PATH)/joystick_virt_kbd.service
	sudo rm -rf $(PRIVATE_DATA_PATH) $(APP_INSTALL_PATH)
	sudo systemctl daemon-reload

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCDIRS) -c $< -o $@


%.moc.cpp: %.h
	$(MOC) $(INCDIRS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) src/*.o