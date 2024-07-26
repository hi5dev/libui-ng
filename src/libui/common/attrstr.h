#pragma once

#include "ui.h"

#ifdef __cplusplus
#define API extern "C"
#else
#define API
#endif

typedef struct uiprivAttrList  uiprivAttrList;
typedef struct uiprivGraphemes uiprivGraphemes;

struct uiprivGraphemes
{
  size_t  len;
  size_t *pointsToGraphemes;
  size_t *graphemesToPoints;
};

API uiAttribute *uiprivAttributeRetain (uiAttribute *a);

API uiprivAttrList *uiprivNewAttrList (void);

API uiprivGraphemes *uiprivNewGraphemes (void *s, size_t len);

API size_t *uiprivAttributedStringCopyUTF16ToUTF8Table (const uiAttributedString *s, size_t *n);

API size_t *uiprivAttributedStringCopyUTF8ToUTF16Table (const uiAttributedString *s, size_t *n);

API size_t uiprivAttributedStringUTF16Len (const uiAttributedString *s);

API size_t uiprivAttributedStringUTF8ToUTF16 (const uiAttributedString *s, size_t n);

API const uint16_t *uiprivAttributedStringUTF16String (const uiAttributedString *s);

API int uiprivAttributeEqual (const uiAttribute *a, const uiAttribute *b);

API int uiprivGraphemesTakesUTF16 (void);

API int uiprivOpenTypeFeaturesEqual (const uiOpenTypeFeatures *a, const uiOpenTypeFeatures *b);

API void uiprivAttrListForEach (const uiprivAttrList *alist, const uiAttributedString *s,
                                uiAttributedStringForEachAttributeFunc f, void *data);

API void uiprivAttrListInsertAttribute (uiprivAttrList *alist, uiAttribute *val, size_t start, size_t end);

API void uiprivAttrListInsertCharactersExtendingAttributes (const uiprivAttrList *alist, size_t start, size_t count);

API void uiprivAttrListInsertCharactersUnattributed (uiprivAttrList *alist, size_t start, size_t count);

API void uiprivAttrListRemoveAttribute (uiprivAttrList *alist, uiAttributeType type, size_t start, size_t end);

API void uiprivAttrListRemoveAttributes (uiprivAttrList *alist, size_t start, size_t end);

API void uiprivAttrListRemoveCharacters (uiprivAttrList *alist, size_t start, size_t end);

API void uiprivAttributeRelease (uiAttribute *a);

API void uiprivFreeAttrList (uiprivAttrList *alist);
