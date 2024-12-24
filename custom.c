#include <fcntl.h>

#define KEYBIND_FN(name) void name(const Arg *a)

#define CLAMP(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define CLAMP01(x)     CLAMP(x, 0, 1)

typedef float f32;

struct backlight_arg {
	const char *backlight; /* /sys/class/backlight/XXX */
	float       delta;     /* [%] */
};

static long
read_long(int fd)
{
	char buffer[64];
	ptrdiff_t rlen = read(fd, buffer, sizeof(buffer));
	close(fd);
	if (rlen == -1)
		return 0;
	if (rlen >= sizeof(buffer))
		rlen = sizeof(buffer) - 1;
	buffer[rlen] = 0;
	return strtol(buffer, 0, 10);
}

static KEYBIND_FN(backlight)
{
	const struct backlight_arg *ba = a->v;
	static f32 max_brightness;

	char path[1024];
	/* NOTE(rnp): this could simplified using the my stream system */
	if (!max_brightness) {
		snprintf(path, sizeof(path), "/sys/class/backlight/%s/max_brightness", ba->backlight);
		int fd = open(path, O_RDONLY);
		if (fd != -1)
			max_brightness = read_long(fd);
	}

	long current_brightness;
	snprintf(path, sizeof(path), "/sys/class/backlight/%s/brightness", ba->backlight);
	int fd = open(path, O_RDONLY);
	if (fd != -1) {
		current_brightness = read_long(fd);
		close(fd);
		fd = open(path, O_WRONLY);
		if (fd != -1 && max_brightness) {
			f32 current_percent = 100 * current_brightness / max_brightness;
			current_percent += ba->delta;
			current_percent  = CLAMP(current_percent, 1, 100);
			current_percent /= 100;
			long len = snprintf(path, sizeof(path), "%ld\n",
			                    (long)(current_percent * max_brightness));
			write(fd, path, len);
			close(fd);
		}
	}
}
