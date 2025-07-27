################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Snake.cpp \
../src/snake2D.cpp 

CPP_DEPS += \
./src/Snake.d \
./src/snake2D.d 

OBJS += \
./src/Snake.o \
./src/snake2D.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++26 -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -v -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Snake.d ./src/Snake.o ./src/snake2D.d ./src/snake2D.o

.PHONY: clean-src

