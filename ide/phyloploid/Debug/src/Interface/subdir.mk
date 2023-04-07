################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/mike/repos/phyloploid_lib/src/Interface/NetDivInterface.cpp 

CPP_DEPS += \
./src/Interface/NetDivInterface.d 

OBJS += \
./src/Interface/NetDivInterface.o 


# Each subdirectory must supply rules for building sources it contributes
src/Interface/NetDivInterface.o: /Users/mike/repos/phyloploid_lib/src/Interface/NetDivInterface.cpp src/Interface/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../api" -I/Users/mike/libraries/boost_1_79_0 -I/Users/mike/libraries/eigen-3.4.0 -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-Interface

clean-src-2f-Interface:
	-$(RM) ./src/Interface/NetDivInterface.d ./src/Interface/NetDivInterface.o

.PHONY: clean-src-2f-Interface

