################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/mike/repos/phyloploid_lib/src/Data/Structure/Edge.cpp \
/Users/mike/repos/phyloploid_lib/src/Data/Structure/Network.cpp \
/Users/mike/repos/phyloploid_lib/src/Data/Structure/Node.cpp 

CPP_DEPS += \
./src/Data/Structure/Edge.d \
./src/Data/Structure/Network.d \
./src/Data/Structure/Node.d 

OBJS += \
./src/Data/Structure/Edge.o \
./src/Data/Structure/Network.o \
./src/Data/Structure/Node.o 


# Each subdirectory must supply rules for building sources it contributes
src/Data/Structure/Edge.o: /Users/mike/repos/phyloploid_lib/src/Data/Structure/Edge.cpp src/Data/Structure/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../api" -I/Users/mike/libraries/boost_1_79_0 -I/Users/mike/libraries/eigen-3.4.0 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../src" -O3 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Data/Structure/Network.o: /Users/mike/repos/phyloploid_lib/src/Data/Structure/Network.cpp src/Data/Structure/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../api" -I/Users/mike/libraries/boost_1_79_0 -I/Users/mike/libraries/eigen-3.4.0 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../src" -O3 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Data/Structure/Node.o: /Users/mike/repos/phyloploid_lib/src/Data/Structure/Node.cpp src/Data/Structure/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../api" -I/Users/mike/libraries/boost_1_79_0 -I/Users/mike/libraries/eigen-3.4.0 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../src" -O3 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-Data-2f-Structure

clean-src-2f-Data-2f-Structure:
	-$(RM) ./src/Data/Structure/Edge.d ./src/Data/Structure/Edge.o ./src/Data/Structure/Network.d ./src/Data/Structure/Network.o ./src/Data/Structure/Node.d ./src/Data/Structure/Node.o

.PHONY: clean-src-2f-Data-2f-Structure

