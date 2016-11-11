################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../m_transf.cpp 

OBJS += \
./m_transf.o 

CPP_DEPS += \
./m_transf.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/ivan/Документы/complexm -I/home/ivan/Загрузки/gtest-1.6.0/include -I/home/ivan/Загрузки/gtest-1.6.0 -includecomplexm.c -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


