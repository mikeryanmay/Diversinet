################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/mike/repos/phyloploid_lib/src/PhyloploidInterface.cpp 

CPP_DEPS += \
./src/PhyloploidInterface.d 

OBJS += \
./src/PhyloploidInterface.o 


# Each subdirectory must supply rules for building sources it contributes
src/PhyloploidInterface.o: /Users/mike/repos/phyloploid_lib/src/PhyloploidInterface.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/mike/repos/phyloploid_lib/ide/phyloploid/../../api" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/PhyloploidInterface.d ./src/PhyloploidInterface.o

.PHONY: clean-src

