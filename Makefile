NAME:=cluscheme
BUILD:=.build
IDIR:=src
ODIR:=$(BUILD)/cache
BIN:=$(BUILD)/bin/$(NAME)

override CXXFLAGS+=-Wall -Wextra -pedantic -ggdb -I$(IDIR) -Ofast -std=c++20
override LDFLAGS+=

SRCS:=$(IDIR)/$(NAME).cc                           \
      $(IDIR)/$(NAME)/ast.cc                       \
      $(IDIR)/$(NAME)/lexer.cc                     \
      $(IDIR)/$(NAME)/lexer/keywords.cc            \
      $(IDIR)/$(NAME)/lexer/token.cc

OBJS:=$(SRCS:$(IDIR)/%.cc=$(ODIR)/%.o)

$(BIN): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

$(ODIR)/$(NAME).o: $(IDIR)/$(NAME).cc
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c -o $@ $<

$(ODIR)/%.o: $(IDIR)/%.cc $(IDIR)/%.hh
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c -o $@ $<

run: $(BIN)
	./$(BIN)

.PHONY: clean compile_flags
clean: ; $(RM) -r $(BUILD)
compile_flags: ; @echo $(CXXFLAGS) | tr ' ' '\n' > compile_flags.txt
