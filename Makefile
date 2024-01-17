NAME:=cluscheme
BUILD:=.build
IDIR:=src
ODIR:=$(BUILD)/cache
BIN:=$(BUILD)/bin/$(NAME)

override CXXFLAGS+=-Wall -Wextra -pedantic -ggdb -I$(IDIR) -Ofast -std=c++23
override LDFLAGS+=

SRCS:=$(IDIR)/$(NAME).cc                               \
      $(IDIR)/$(NAME)/tokenizer.cc

OBJS:=$(SRCS:$(IDIR)/%.cc=$(ODIR)/%.o)

$(BIN): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

$(ODIR)/%.o: $(IDIR)/%.cc $(IDIR)/%.hh
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c -o $@ $<

$(ODIR)/%.o: $(IDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c -o $@ $<

run: $(BIN)
	./$(BIN)

.PHONY: clean compile_flags
clean: ; $(RM) -r $(BUILD)
compile_flags: ; @echo $(CFLAGS) | tr ' ' '\n' > compile_flags.txt
