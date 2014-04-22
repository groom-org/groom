#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dsh/dsh.h>

#ifdef DSH_LOCAL
#include <dsh/dummy.h>
#else
#include <avr/io.h>
#endif /* DSH_LOCAL */

#include <dsh/rw_entries.h>


static void dsh_ddr(dsh_char_writer writer);
static void dsh_ddr_letter(dsh_char_writer writer, volatile uint8_t ddr, char letter);
static int dsh_get_line(dsh_char_reader reader, char *line, size_t size);
static int dsh_printf(dsh_char_writer writer, const char *format_string, ...);
static void dsh_r(dsh_char_writer writer, const char *arg);
static void dsh_write_str(dsh_char_writer writer, const char *string);

void dsh_init(struct dsh_shell *shell,
	      dsh_char_reader reader,
	      dsh_char_writer writer)
{
	shell->reader = reader;
	shell->writer = writer;
}

int dsh_run(struct dsh_shell *shell)
{
	char line[256];
	int res;

	for(;;) {
		/* print the prompt */
		dsh_write_str(shell->writer, "[dsh]$ ");
		/* get a line */
		res = dsh_get_line(shell->reader, line, sizeof(line));
		if (res < 0) {
			dsh_write_str(shell->writer, "dsh: input line too long\r\n");
			continue;
		}

		char *command;
		char *args;

		/* the entire line is the command (for now) */
		command = line;
		/* find the location of the first space. args will be null if there is none */
		args = strchr(line, ' ');

		if (args) {
			*args = '\0';
			args++;

			/* skip any additional spaces */
			while (*args == ' ' && *args != '\0') {
				args++;
			}

			if (*args == '\0') {
				/* 
				 * if this is the case then there are no additional args,
				 * so set args to NULL
				 */
				args = NULL;
			}
		}

		if (strcmp(command, "echo") == 0) {
			if (args) {
				dsh_printf(shell->writer, "%s\r\n", args);
			}
			continue;
		} 

		if (strcmp(command, "ddr") == 0) {
			dsh_ddr(shell->writer);
			continue;
		}

		if (strcmp(command, "r") == 0) {
			dsh_r(shell->writer, args);
			continue;
		}

		/*
		 * return to whoever called us with the status given
		 */
		if (strcmp(command, "exit") == 0) {
			int retval = 0;
			if (args) {
				retval = atoi(args);
			}

			return retval;
		}

		dsh_printf(shell->writer, "dsh: %s: command not found\r\n", command);
	}
}

/* Static functions */

static void dsh_ddr(dsh_char_writer writer)
{
	/* print the header */
	dsh_printf(writer, "DDR 7 6 5 4 3 2 1 0\r\n");
	dsh_ddr_letter(writer, DDRB, 'B');
	dsh_ddr_letter(writer, DDRC, 'C');
	dsh_ddr_letter(writer, DDRD, 'D');
}

static void dsh_ddr_letter(dsh_char_writer writer, volatile uint8_t ddr, char letter)
{
	int i;

	dsh_printf(writer, "  %c", letter);
	for (i = 0x80; i != 0; i >>= 1) {
		if (ddr & i) {
			dsh_printf(writer, " O");
		} else {
			dsh_printf(writer, " I");
		}
	}
	dsh_printf(writer, "\r\n");
}

/*
 * Read a from reader until we see a new line and then save the
 * result into line. The resulting value at line is garaunteed
 * to be a null terminated string.
 *
 * If we failed ran out of space on the reader string then we return
 * a value of -1 (but only after we see a newline). Otherwise the
 * return value is 0
 */
static int dsh_get_line(dsh_char_reader reader, char *line, size_t size)
{
	size_t n = 0;
	char c;
	int overrun = 0;

	while ( (c = reader()) != '\n') {
		if (n < (size - 1)) {
			line[n] = c;
			n++;
		} else {
			overrun = 1;
		}
	}

	line[n] = '\0';

	if (overrun) {
		return -1;
	}

	return 0;
}

static int dsh_printf(dsh_char_writer writer, const char *format_string, ...)
{
	va_list varargs;
	char *string = NULL;
	int size;

	va_start(varargs, format_string);
	size = vsnprintf(string, 0, format_string, varargs);
	va_end(varargs);
	
	string = malloc(size + 1);
	if (!string) {
		return -1;
	}

	va_start(varargs, format_string);
	vsnprintf(string, size + 1, format_string, varargs);
	va_end(varargs);

	dsh_write_str(writer, string);
	free(string);

	return 0;
}

static void dsh_r(dsh_char_writer writer, const char *arg)
{
	int i;

	for (i = 0; i < num_rw_entries; i++) {
		if (!strcmp(rw_entries[i].name, arg)) {
			int val = *rw_entries[i].ptr;
			int signed_val = (int8_t)(val);
			dsh_printf(writer,
					"unsigned: %u\r\n"
					"signed:   %d\r\n"
					"hex:      0x%x\r\n",
					val,
					signed_val,
					val);
			return;
		}
	}

	dsh_printf(writer, "no variable named '%s'\r\n", arg);
}

static void dsh_write_str(dsh_char_writer writer, const char *string)
{
	while (*string != '\0') {
		writer(*string);
		string++;
	}
}
