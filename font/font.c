#include "font.h"
#include <fontconfig/fontconfig.h>
#include <stdio.h>
#include <stdlib.h>

Font register_font(NVGcontext *vg, const char *name, const char *filename, float heightFactor, float heightOffset) {
  Font font = {
    .name = name,
    .heightFactor = heightFactor,
    .heightOffset = heightOffset,
  };
	font.handle = nvgCreateFont(vg, name, filename);
  return font;
}

Font load_font(NVGcontext *vg, char * input_pattern, float heightFactor, float heightOffset) {
  FcInit();
  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcNameParse((const FcChar8*)input_pattern);
  FcConfigSubstitute(config, pattern, FcMatchPattern);
  FcDefaultSubstitute(pattern);

  char *fontFile = NULL;
  Font font = {0};
  FcResult result;
  FcPattern *font_pattern = FcFontMatch(config, pattern, &result);
  if (font_pattern) {
    FcChar8 *file = NULL;
    if (FcPatternGetString(font_pattern, FC_FILE, 0, &file) == FcResultMatch) {
      fontFile = (char*) file;
      font = register_font(vg, input_pattern, fontFile, heightFactor, heightOffset);
    } else {
      printf("No font matches!\n");
      exit(1);
    }
  } else {
    printf("Font pattern not found!\n");
    exit(1);
  }

  FcPatternDestroy(font_pattern);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  FcFini();
  return font;
}


