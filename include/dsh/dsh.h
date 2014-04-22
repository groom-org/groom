#ifndef DSH_DSH_H__
#define DSH_DSH_H__

typedef char (*dsh_char_reader)(void);
typedef void (*dsh_char_writer)(char c);

struct dsh_shell {
	dsh_char_reader reader;
	dsh_char_writer writer;
};

void dsh_init(struct dsh_shell *shell,
	      dsh_char_reader reader,
	      dsh_char_writer writer);

int dsh_run(struct dsh_shell *shell);

#endif /* DSH_DSH_H__ */
