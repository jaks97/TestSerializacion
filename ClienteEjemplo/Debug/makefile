################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ClienteEjemplo

dependents:
	-cd /home/utnso/workspace/serializacion/Debug && $(MAKE) clean all

# Tool invocations
ClienteEjemplo: $(OBJS) $(USER_OBJS) /home/utnso/workspace/serializacion/Debug/libserializacion.so
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc -L"/home/utnso/workspace/serializacion/Debug" -o "ClienteEjemplo" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) ClienteEjemplo
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/serializacion/Debug/libserializacion.so:

-include ../makefile.targets
