CC      = cc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic \
           -I. -Iunity -Istubs \
           -DUNITY_OUTPUT_COLOR

UNITY_SRC = unity/unity.c

.PHONY: all test clean

all: test

test: test_messages test_gripper
	@echo ""
	@echo "Running test_messages..."
	@./test_messages
	@echo ""
	@echo "Running test_gripper..."
	@./test_gripper

test_messages: test_messages.c $(UNITY_SRC)
	$(CC) $(CFLAGS) $^ -o $@

test_gripper: test_gripper.c $(UNITY_SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f test_messages test_gripper
