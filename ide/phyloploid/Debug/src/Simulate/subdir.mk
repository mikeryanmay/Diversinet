################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/mike/repos/phyloploid_lib/src/Simulate/BaseSimulator.cpp 

CPP_DEPS += \
./src/Simulate/BaseSimulator.d 

OBJS += \
./src/Simulate/BaseSimulator.o 


# Each subdirectory must supply rules for building sources it contributes
src/Simulate/BaseSimulator.o: /Users/mike/repos/phyloploid_lib/src/Simulate/BaseSimulator.cpp src/Simulate/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../api" -I/Users/mike/libraries/boost_1_79_0 -I/Users/mike/libraries/eigen-3.4.0 -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-Simulate

clean-src-2f-Simulate:
	-$(RM) ./src/Simulate/BaseSimulator.d ./src/Simulate/BaseSimulator.o

.PHONY: clean-src-2f-Simulate

