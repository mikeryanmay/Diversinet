################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/mike/repos/phyloploid_lib/sim/main.cpp 

CPP_DEPS += \
./sim/main.d 

OBJS += \
./sim/main.o 


# Each subdirectory must supply rules for building sources it contributes
sim/main.o: /Users/mike/repos/phyloploid_lib/sim/main.cpp sim/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/mike/repos/phyloploid_lib/ide/Test/../../api/" -I/Users/mike/libraries/boost_1_79_0 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-sim

clean-sim:
	-$(RM) ./sim/main.d ./sim/main.o

.PHONY: clean-sim

