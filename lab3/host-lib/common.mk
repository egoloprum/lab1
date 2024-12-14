CC = g++
AR = ar
RM = rm -f
RMDIR = rm -rf

# host compiler global settings
CXXFLAGS += -std=gnu++20 -O3 -g -Wall -c -fmessage-length=0 -fPIC
CFLAGS += -O3 -g -Wall -fmessage-length=0 
