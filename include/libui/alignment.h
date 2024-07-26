#pragma once

/**
 * @brief Alignment specifiers to define placement within the reserved area.
 */
typedef enum uiAlign
{
  uiAlignFill,   //!< Fill area.
  uiAlignStart,  //!< Place at start.
  uiAlignCenter, //!< Place in center.
  uiAlignEnd,    //!< Place at end.
} uiAlign;

/**
 * @brief Placement specifier to define placement in relation to another control.
 */
typedef enum uiAt
{
  uiAtLeading,  //!< Place before control.
  uiAtTop,      //!< Place above control.
  uiAtTrailing, //!< Place behind control.
  uiAtBottom,   //!< Place below control.
} uiAt;
