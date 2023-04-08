################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/mike/repos/phyloploid_lib/calc/main.cpp 

CPP_DEPS += \
./calc/main.d 

OBJS += \
./calc/main.o 


# Each subdirectory must supply rules for building sources it contributes
calc/main.o: /Users/mike/repos/phyloploid_lib/calc/main.cpp calc/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/Users/mike/libraries/boost_1_79_0 -I"/Users/mike/repos/phyloploid_lib/ide/Calc/../../api/" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-calc

clean-calc:
	-$(RM) ./calc/main.d ./calc/main.o

.PHONY: clean-calc

