# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Manually listed source files
SRC += $(SRC_DIR)/00_main.cpp
SRC += $(SRC_DIR)/Base_bag.cpp
SRC += $(SRC_DIR)/Derived1_searchable_bag.cpp
SRC += $(SRC_DIR)/Derived2A_array_bag.cpp
SRC += $(SRC_DIR)/Derived2T_tree_bag.cpp
SRC += $(SRC_DIR)/UnitedA_searchable_array_bag.cpp
SRC += $(SRC_DIR)/UnitedT_searchable_tree_bag.cpp
SRC += $(SRC_DIR)/Wrapper_set.cpp

# Derived object files
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
TARGET = app

# Default target
all: $(TARGET)

# Create binary
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files
clean:
	@rm -rf $(OBJ_DIR)

# Full clean: object files + binary
fclean: clean
	@rm -rf $(TARGET)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re
