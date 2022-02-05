#include "widgets.h"

point_t text_draw(AppContext *app, text_t *conf) {
  nvgBeginPath(app->vg);
	nvgFillColor(app->vg, conf->color);
	nvgFontSize(app->vg, (float)conf->size/conf->font->heightFactor);
	nvgFontFace(app->vg, conf->font->name);
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  float bounds[4];
	nvgTextBounds(app->vg,
      conf->widget.position.x,
      conf->widget.position.y - (conf->size * conf->font->heightOffset),
      conf->content,
      NULL,
      bounds);

	nvgText(app->vg,
      conf->widget.position.x,
      conf->widget.position.y - (conf->size * conf->font->heightOffset),
      conf->content,
      NULL);

  return (point_t){
    .x = bounds[2] - bounds[0],
    .y = conf->size,
  };
}

point_t text_size(AppContext *app, text_t *conf) {
	nvgFontSize(app->vg, (float)conf->size/conf->font->heightFactor);
	nvgFontFace(app->vg, conf->font->name);
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  float bounds[4];
	nvgTextBounds(app->vg,
      conf->widget.position.x,
      conf->widget.position.y - (conf->size * conf->font->heightOffset),
      conf->content,
      NULL,
      bounds);

  return (point_t){
    .x = bounds[2] - bounds[0],
    .y = conf->size,
  };
}
