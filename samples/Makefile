#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
all:
	@$(MAKE) -C alphatest --no-print-directory
	@$(MAKE) -C printtest --no-print-directory
	@$(MAKE) -C dialogtest --no-print-directory
	@$(MAKE) -C soundtest --no-print-directory

clean:
	@$(MAKE) -C alphatest clean --no-print-directory
	@$(MAKE) -C printtest clean --no-print-directory
	@$(MAKE) -C dialogtest clean --no-print-directory
	@$(MAKE) -C soundtest clean --no-print-directory
