#include "PS17_AT90.h"

//added by xc 
bool is_alt_tab_active = false; // Super Alt Tab Code
uint16_t alt_tab_timer = 0;
#ifdef VIA_ENABLE
	enum custom_keycodes { //Use USER 00 instead of SAFE_RANGE for Via. VIA json must include the custom keycode.
	  ATABF = USER00, //Alt tab forwards
	  ATABR //Alt tab reverse
	};
#else
	enum custom_keycodes { //Use USER 00 instead of SAFE_RANGE for Via. VIA json must include the custom keycode.
	  ATABF = SAFE_RANGE, //Alt tab forwards
	  ATABR //Alt tab reverse
	};
#endif
//ended by xc

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	KEYMAP(
		KC_VOLD, MO(1), KC_VOLU, 
		KC_NLCK, KC_PSLS, KC_PAST, KC_PMNS, 
		KC_P7, KC_P8, KC_P9, KC_PPLS, 
		KC_P4, KC_P5, KC_P6, 
		KC_P1, KC_P2, KC_P3, KC_PENT, 
		KC_P0, KC_PDOT),

	KEYMAP(
		KC_A, KC_TRNS, KC_B, 
		RGB_TOG, RGB_MOD, RGB_VAD, RGB_VAI, 
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
		KC_TRNS, KC_TRNS, KC_TRNS, 
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
		KC_TRNS, KC_TRNS)
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {


	switch (id) {

	}
	return MACRO_NONE;
}

void matrix_init_user(void) {
}

//void matrix_scan_user(void) {
//}


//below added by xc replace above part 2
void matrix_scan_user(void) { //run whenever user matrix is scanned
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 1000) {
      unregister_code(KC_LALT);
      is_alt_tab_active = false;
    }
  }
}
//above added by xc part 2


//bool process_record_user(uint16_t keycode, keyrecord_t *record) {
//	return true;
//}

//below added by xc part 3
bool process_record_user(uint16_t keycode, keyrecord_t *record) { //Actions to override existing key behaviours
	switch (keycode) { //For super alt tab keycodes
	case ATABF:	//Alt tab forwards
	  if (record->event.pressed) {
		if (!is_alt_tab_active) {
		  is_alt_tab_active = true;
		  register_code(KC_LALT);
		}
		alt_tab_timer = timer_read();
		register_code(KC_TAB);
	  } else {
		unregister_code(KC_TAB);
	  }
	  break;
	case ATABR:	//Alt tab reverse
	  if (record->event.pressed) {
		if (!is_alt_tab_active) {
		  is_alt_tab_active = true;
		  register_code(KC_LALT);
		}
		alt_tab_timer = timer_read();
		register_code(KC_LSHIFT);
		register_code(KC_TAB);
	  } else {
		unregister_code(KC_LSHIFT);
		unregister_code(KC_TAB);
	  }
	  break;
	}
	return true;
};
//above by xc part 3


// void encoder_update_user(uint8_t index, bool clockwise) {
//     if (index == 0) { /* First encoder */
//         if (clockwise) {
//             tap_code(dynamic_keymap_get_keycode(biton32(layer_state), 0, 0));
//         } else {
//             tap_code(dynamic_keymap_get_keycode(biton32(layer_state), 0, 2));
//         }
//     }
// }

void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        if (clockwise) {
        	action_exec((keyevent_t){
        					.key = (keypos_t){.row = 0, .col = 0}, .pressed = true, .time = (timer_read() | 1)  /* time should not be 0 */
                       });  
        	action_exec((keyevent_t){
        					.key = (keypos_t){.row = 0, .col = 0}, .pressed = false, .time = (timer_read() | 1)  /* time should not be 0 */
                       });  
           } else {
        	action_exec((keyevent_t){
        					.key = (keypos_t){.row = 0, .col = 2}, .pressed = true, .time = (timer_read() | 1)  /* time should not be 0 */
                       });
        	action_exec((keyevent_t){
        					.key = (keypos_t){.row = 0, .col = 2}, .pressed = false, .time = (timer_read() | 1)  /* time should not be 0 */
                       });  
           }
    }
}

// 以下为RGB灯效分层设置：--------------------------------------------------------------------------------
// Light LEDs 0 to 6 in YELLOW when caps lock is active. Hard to ignore!
const rgblight_segment_t PROGMEM my_capslock_layer[] = RGBLIGHT_LAYER_SEGMENTS({0, 0, HSV_WHITE});
// Light LEDs 0 to 6 in RED when keyboard layer 1 is active
const rgblight_segment_t PROGMEM my_layer1_layer[] = RGBLIGHT_LAYER_SEGMENTS({0, 17, HSV_BLUE});
// Light LEDs 6 to 11 in GREEN when keyboard layer 2 is active
const rgblight_segment_t PROGMEM my_layer2_layer[] = RGBLIGHT_LAYER_SEGMENTS({0, 17, HSV_RED});

const rgblight_segment_t *const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
	my_capslock_layer,
    my_layer1_layer,
    my_layer2_layer
   // Overrides other layers
);

void keyboard_post_init_user(void)
{    // Enable the LED layers
    rgblight_layers = my_rgb_layers;
}

layer_state_t layer_state_set_user(layer_state_t state)
{    // Both layers will light up if both kb layers are active
    rgblight_set_layer_state(1, layer_state_cmp(state, 1));
    rgblight_set_layer_state(2, layer_state_cmp(state, 2));
    return state;
}


bool led_update_user(led_t led_state)
{
   rgblight_set_layer_state(0, led_state.caps_lock);
    return true;
}

// 以上为RGB灯效分层设置：--------------------------------------------------------------------------------



void led_set_user(uint8_t usb_led) {

	if (usb_led & (1 << USB_LED_NUM_LOCK)) {
		DDRD |= (1 << 0); PORTD &= ~(1 << 0);
	} else {
		DDRD &= ~(1 << 0); PORTD &= ~(1 << 0);
	}

	if (usb_led & (1 << USB_LED_CAPS_LOCK)) {
		DDRD |= (1 << 5); PORTD &= ~(1 << 5);
	} else {
		DDRD &= ~(1 << 5); PORTD &= ~(1 << 5);
	}

	if (usb_led & (1 << USB_LED_SCROLL_LOCK)) {
		DDRD |= (1 << 4); PORTD &= ~(1 << 4);
	} else {
		DDRD &= ~(1 << 4); PORTD &= ~(1 << 4);
	}
	if (usb_led & (1 << USB_LED_COMPOSE)) {
		
	} else {
		
	}

	if (usb_led & (1 << USB_LED_KANA)) {
		
	} else {
		
	}

}
//2021.11.2, 原capslock=d5, scrolllock=d4;
