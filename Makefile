# Makefile preamble
MAKEFLAGS += --warn-undefined-variables
SHELL := bash
# .SHELLFLAGS := -eu -o pipefail -c
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
.SUFFIXES:

define F_QUIET_TOOL
$(if ${verbose},,@printf '  %-7s %s\n' $1 $@;)
endef

# Utility variables reducing output verbosity. Place these variables in front of
# calls to their respective tools (e.g. quiet_cc in front of CC) and when
# "verbose" is not set then any calls to CC display "CC <target>" instead of the
# full command string. Set "verbose" to any value to display the full command.
verbose       ?=
# verbose       := true
quiet_ruby    = $(call F_QUIET_TOOL,"RUBY")
quiet_mkdir    = $(call F_QUIET_TOOL,"MKDIR")

define ET_DEP_RULE
    $$(sort ${1}):
		$${quiet_mkdir}mkdir -p $$@
endef

# include tools/framework.mk
# verbose := true

# ------------------------------------------------------------------------------
# Firmware Configuration
# target       := index.html
builddir     := site

tgt_srcs     := src/avr.md

# Configure tooling
RUBY := ruby

objdir     := ${builddir}
tgtdir     := ${tgt_bindir}

tgt_objs   := $(addprefix ${objdir}/,$(addsuffix .html,$(basename ${tgt_srcs})))

depdirs    := ${tgtdir} ${objdir}
depdirs    += $(dir ${tgt_objs})

# Add order-only prerequisite rules for depdirs
$(foreach dir,${depdirs},$(eval $(filter ${dir}%,${tgt_objs}): | ${dir}))

# tgt := $(strip ${target})

.PHONY: all
all: ${tgt_objs}

${tgtdir}/${tgt_bin}: ${tgtdir}/${tgt_elf} | ${tgtdir}


# Markdown build rule
${objdir}/%.html : %.md
	${quiet_ruby}$(strip ${RUBY} tools/build.rb -i $< -o $@)

.PHONY: clean
clean:
	${RM} ${tgt_objs}

# .PHONY: distclean
# distclean:
# 	${RM} -rf ${depdirs}

print.%:
	@echo '$*=$($*)'
	@echo '  origin = $(origin $*)'
	@echo '  flavor = $(flavor $*)'
	@echo '   value = $(value  $*)'

# Create dependent directories
$(eval $(call ET_DEP_RULE,${depdirs}))

# Include automatically generated dependencies
# -include ${tgt_objs:%.o=%.d}
