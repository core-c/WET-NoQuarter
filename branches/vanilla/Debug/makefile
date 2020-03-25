################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/ui/subdir.mk
-include src/game/subdir.mk
-include src/cgame/subdir.mk
-include src/botai/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: 260_pure_ET_bugfix

# Tool invocations
260_pure_ET_bugfix: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o"260_pure_ET_bugfix" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(EXECUTABLES)$(C_DEPS) 260_pure_ET_bugfix
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
