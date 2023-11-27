all:
	$(MAKE) -C node
	$(MAKE) -C root

fmt:
	clang-format -i --style=file -- *.[ch] */*.[ch]

.SILENT:
.PHONY: all fmt
