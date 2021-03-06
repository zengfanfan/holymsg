define PRINT_COLOR
	printf "\033[1;40;32m  %8s \033[1;40;36m %s \033[0m\r\n" $(1) $(2)
endef

define _NOP_
	@echo $$* >> /dev/null
endef

define CC
	@$(call PRINT_COLOR, CC, $@)
	@gcc
endef

define LD
	@$(call PRINT_COLOR, LD, $@)
	@ld
endef

define AR
	@$(call PRINT_COLOR, AR, $@)
	@ar
endef

define AS
	@$(call PRINT_COLOR, AS, $@)
	@as
endef

ifeq ($(_DEBUG_),y)
STRIP := $(_NOP_)
else
define STRIP
	@$(call PRINT_COLOR, STRIP, $@)
	@strip
endef
endif

define INSTALL
	@-_my_func_() \
	{\
		$(call PRINT_COLOR, INSTALL, $$1);\
		cp -f $$*;\
	};\
	_my_func_
endef

define RM
	@-_my_func_() \
	{\
		$(call PRINT_COLOR, RM, "$$*");\
		rm -f $$*;\
	};\
	_my_func_
endef

