#include <sound/acs422.h>

struct acs422_preset acs422_presets[] = {
		{
				.name = "BeatsOnSpk",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1 
						{ 0x21, 0xa0}, // Gain Control 
						//{ 0x22, 0x7f}, // Constant Output Power 1 
						{ 0x23, 0x02}, // Constant Output Power 2 
						{ 0x25, 0x03}, // Comp/Lim/Exp (CLE) Control 
						{ 0x26, 0x02}, // CLE Make-up Gain 
						{ 0x27, 0xe4}, // Compressor Threshold 
						{ 0x28, 0x02}, // Compressor Ratio 
						{ 0x29, 0x72}, // Compressor Attack Time (Low) 
						{ 0x2a, 0xf7}, // Compressor Attack Time (High) 
						{ 0x2b, 0xb8}, // Compressor Release Time (Low) 
						{ 0x2c, 0xfb}, // Compressor Release Time (High) 
						{ 0x2d, 0xf9}, // Limiter Threshold 
						{ 0x2e, 0xf9}, // Limiter Target 
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low) 
						{ 0x30, 0xd5}, // Limiter Attack Time (High) 
						{ 0x31, 0x72}, // Limiter Release Time (Low) 
						{ 0x32, 0xf7}, // Limiter Release Time (High) 
						{ 0x33, 0xff}, // Expander Threshold 
						{ 0x34, 0x01}, // Expander Ratio 
						{ 0x35, 0x74}, // Expander Attack Time (Low) 
						{ 0x36, 0xaa}, // Expander Attack Time (High) 
						{ 0x37, 0x74}, // Expander Release Time (Low) 
						{ 0x38, 0xaa}, // Expander Release Time (High) 
						{ 0x39, 0x00}, // Effects Control 
						//{ 0x02, 0x7f}, // SPKVOL Left
						//{ 0x03, 0x7f}, // SPKVOL Right
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x003e7fc3, 0xff830079, 0x003e7fc3, 0x007cf560, 0xffc2f653, // EQ0, Channel 0, Filter 0
						0x003bde73, 0xff884318, 0x003bde73, 0x0077708f, 0xffc7f6bf, // EQ0, Channel 0, Filter 1
						0x003b70c1, 0xffaa7682, 0x00324726, 0x0055897e, 0xffd24819, // EQ0, Channel 0, Filter 2
						0x0038576e, 0xffc00f51, 0x002e35e4, 0x003ff0af, 0xffd972ad, // EQ0, Channel 0, Filter 3
						0x003dd470, 0xff8bc8ea, 0x003840c7, 0x00743716, 0xffc9eac8, // EQ0, Channel 0, Filter 4
						0x00400000, 0xff85c1d5, 0x003b0a8d, 0x007a3e2b, 0xffc4f573, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale 
						0x003e7fc3, 0xff830079, 0x003e7fc3, 0x007cf560, 0xffc2f653, // EQ0, Channel 1, Filter 0
						0x003bde73, 0xff884318, 0x003bde73, 0x0077708f, 0xffc7f6bf, // EQ0, Channel 1, Filter 1
						0x003b70c1, 0xffaa7682, 0x00324726, 0x0055897e, 0xffd24819, // EQ0, Channel 1, Filter 2
						0x0038576e, 0xffc00f51, 0x002e35e4, 0x003ff0af, 0xffd972ad, // EQ0, Channel 1, Filter 3
						0x003dd470, 0xff8bc8ea, 0x003840c7, 0x00743716, 0xffc9eac8, // EQ0, Channel 1, Filter 4
						0x00400000, 0xff85c1d5, 0x003b0a8d, 0x007a3e2b, 0xffc4f573, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale 
						0x00400000, 0xff98c2e2, 0x002b93a5, 0x00673d1e, 0xffd46c5b, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale 
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale 
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOnSpk",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1 
						{ 0x21, 0xa0}, // Gain Control 
						//{ 0x22, 0x7f}, // Constant Output Power 1 
						{ 0x23, 0x02}, // Constant Output Power 2 
						{ 0x25, 0x03}, // Comp/Lim/Exp (CLE) Control 
						{ 0x26, 0x02}, // CLE Make-up Gain 
						{ 0x27, 0xe4}, // Compressor Threshold 
						{ 0x28, 0x02}, // Compressor Ratio 
						{ 0x29, 0x23}, // Compressor Attack Time (Low) 
						{ 0x2a, 0xf8}, // Compressor Attack Time (High) 
						{ 0x2b, 0x11}, // Compressor Release Time (Low) 
						{ 0x2c, 0xfc}, // Compressor Release Time (High) 
						{ 0x2d, 0xf9}, // Limiter Threshold 
						{ 0x2e, 0xf9}, // Limiter Target 
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low) 
						{ 0x30, 0xd8}, // Limiter Attack Time (High) 
						{ 0x31, 0x23}, // Limiter Release Time (Low) 
						{ 0x32, 0xf8}, // Limiter Release Time (High) 
						{ 0x33, 0xff}, // Expander Threshold 
						{ 0x34, 0x01}, // Expander Ratio 
						{ 0x35, 0x67}, // Expander Attack Time (Low) 
						{ 0x36, 0xb1}, // Expander Attack Time (High) 
						{ 0x37, 0x67}, // Expander Release Time (Low) 
						{ 0x38, 0xb1}, // Expander Release Time (High) 
						{ 0x39, 0x00}, // Effects Control 
						//{ 0x02, 0x7f}, // SPKVOL Left
						//{ 0x03, 0x7f}, // SPKVOL Right
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x003e9eb2, 0xff82c29a, 0x003e9eb2, 0x007d34d0, 0xffc2ba05, // EQ0, Channel 0, Filter 0
						0x003c3254, 0xff879b56, 0x003c3254, 0x007823e1, 0xffc75a8f, // EQ0, Channel 0, Filter 1
						0x003bc322, 0xffa5c45a, 0x00333f13, 0x005a3ba6, 0xffd0fdcb, // EQ0, Channel 0, Filter 2
						0x0038d993, 0xffb93dac, 0x002f6433, 0x0046c254, 0xffd7c239, // EQ0, Channel 0, Filter 3
						0x003dfe5c, 0xff8ac187, 0x0038d66e, 0x00753e79, 0xffc92b36, // EQ0, Channel 0, Filter 4
						0x00400000, 0xff853f11, 0x003b6e07, 0x007ac0ef, 0xffc491f9, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale 
						0x003e9eb2, 0xff82c29a, 0x003e9eb2, 0x007d34d0, 0xffc2ba05, // EQ0, Channel 1, Filter 0
						0x003c3254, 0xff879b56, 0x003c3254, 0x007823e1, 0xffc75a8f, // EQ0, Channel 1, Filter 1
						0x003bc322, 0xffa5c45a, 0x00333f13, 0x005a3ba6, 0xffd0fdcb, // EQ0, Channel 1, Filter 2
						0x0038d993, 0xffb93dac, 0x002f6433, 0x0046c254, 0xffd7c239, // EQ0, Channel 1, Filter 3
						0x003dfe5c, 0xff8ac187, 0x0038d66e, 0x00753e79, 0xffc92b36, // EQ0, Channel 1, Filter 4
						0x00400000, 0xff853f11, 0x003b6e07, 0x007ac0ef, 0xffc491f9, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale 
						0x00400000, 0xff96b842, 0x002cfe7f, 0x006947be, 0xffd30181, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale 
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale 
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffSpk",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1 
						{ 0x21, 0xa0}, // Gain Control 
						//{ 0x22, 0x7f}, // Constant Output Power 1 
						{ 0x23, 0x02}, // Constant Output Power 2 
						{ 0x25, 0x03}, // Comp/Lim/Exp (CLE) Control 
						{ 0x26, 0x02}, // CLE Make-up Gain 
						{ 0x27, 0xe4}, // Compressor Threshold 
						{ 0x28, 0x02}, // Compressor Ratio 
						{ 0x29, 0x72}, // Compressor Attack Time (Low) 
						{ 0x2a, 0xf7}, // Compressor Attack Time (High) 
						{ 0x2b, 0xb8}, // Compressor Release Time (Low) 
						{ 0x2c, 0xfb}, // Compressor Release Time (High) 
						{ 0x2d, 0xf9}, // Limiter Threshold 
						{ 0x2e, 0xf9}, // Limiter Target 
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low) 
						{ 0x30, 0xd5}, // Limiter Attack Time (High) 
						{ 0x31, 0x72}, // Limiter Release Time (Low) 
						{ 0x32, 0xf7}, // Limiter Release Time (High) 
						{ 0x33, 0xff}, // Expander Threshold 
						{ 0x34, 0x01}, // Expander Ratio 
						{ 0x35, 0x74}, // Expander Attack Time (Low) 
						{ 0x36, 0xaa}, // Expander Attack Time (High) 
						{ 0x37, 0x74}, // Expander Release Time (Low) 
						{ 0x38, 0xaa}, // Expander Release Time (High) 
						{ 0x39, 0x00}, // Effects Control 
						//{ 0x02, 0x7f}, // SPKVOL Left
						//{ 0x03, 0x7f}, // SPKVOL Right
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x003e7fc3, 0xff830079, 0x003e7fc3, 0x007cf560, 0xffc2f653, // EQ0, Channel 0, Filter 0
						0x003bde73, 0xff884318, 0x003bde73, 0x0077708f, 0xffc7f6bf, // EQ0, Channel 0, Filter 1
						0x003b70c1, 0xffaa7682, 0x00324726, 0x0055897e, 0xffd24819, // EQ0, Channel 0, Filter 2
						0x0038576e, 0xffc00f51, 0x002e35e4, 0x003ff0af, 0xffd972ad, // EQ0, Channel 0, Filter 3
						0x003dd470, 0xff8bc8ea, 0x003840c7, 0x00743716, 0xffc9eac8, // EQ0, Channel 0, Filter 4
						0x003bc2a3, 0xff8b5ad4, 0x0039a58f, 0x0074a52c, 0xffca97ce, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale 
						0x003e7fc3, 0xff830079, 0x003e7fc3, 0x007cf560, 0xffc2f653, // EQ0, Channel 1, Filter 0
						0x003bde73, 0xff884318, 0x003bde73, 0x0077708f, 0xffc7f6bf, // EQ0, Channel 1, Filter 1
						0x003b70c1, 0xffaa7682, 0x00324726, 0x0055897e, 0xffd24819, // EQ0, Channel 1, Filter 2
						0x0038576e, 0xffc00f51, 0x002e35e4, 0x003ff0af, 0xffd972ad, // EQ0, Channel 1, Filter 3
						0x003dd470, 0xff8bc8ea, 0x003840c7, 0x00743716, 0xffc9eac8, // EQ0, Channel 1, Filter 4
						0x003bc2a3, 0xff8b5ad4, 0x0039a58f, 0x0074a52c, 0xffca97ce, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale 
						0x00400000, 0xff98c2e2, 0x002b93a5, 0x00673d1e, 0xffd46c5b, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale 
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale 
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffSpk",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1 
						{ 0x21, 0xa0}, // Gain Control 
						//{ 0x22, 0x7f}, // Constant Output Power 1 
						{ 0x23, 0x02}, // Constant Output Power 2 
						{ 0x25, 0x03}, // Comp/Lim/Exp (CLE) Control 
						{ 0x26, 0x02}, // CLE Make-up Gain 
						{ 0x27, 0xe4}, // Compressor Threshold 
						{ 0x28, 0x02}, // Compressor Ratio 
						{ 0x29, 0x23}, // Compressor Attack Time (Low) 
						{ 0x2a, 0xf8}, // Compressor Attack Time (High) 
						{ 0x2b, 0x11}, // Compressor Release Time (Low) 
						{ 0x2c, 0xfc}, // Compressor Release Time (High) 
						{ 0x2d, 0xf9}, // Limiter Threshold 
						{ 0x2e, 0xf9}, // Limiter Target 
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low) 
						{ 0x30, 0xd8}, // Limiter Attack Time (High) 
						{ 0x31, 0x23}, // Limiter Release Time (Low) 
						{ 0x32, 0xf8}, // Limiter Release Time (High) 
						{ 0x33, 0xff}, // Expander Threshold 
						{ 0x34, 0x01}, // Expander Ratio 
						{ 0x35, 0x67}, // Expander Attack Time (Low) 
						{ 0x36, 0xb1}, // Expander Attack Time (High) 
						{ 0x37, 0x67}, // Expander Release Time (Low) 
						{ 0x38, 0xb1}, // Expander Release Time (High) 
						{ 0x39, 0x00}, // Effects Control 
						//{ 0x02, 0x7f}, // SPKVOL Left
						//{ 0x03, 0x7f}, // SPKVOL Right
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x003e9eb2, 0xff82c29a, 0x003e9eb2, 0x007d34d0, 0xffc2ba05, // EQ0, Channel 0, Filter 0
						0x003c3254, 0xff879b56, 0x003c3254, 0x007823e1, 0xffc75a8f, // EQ0, Channel 0, Filter 1
						0x003bc322, 0xffa5c45a, 0x00333f13, 0x005a3ba6, 0xffd0fdcb, // EQ0, Channel 0, Filter 2
						0x0038d993, 0xffb93dac, 0x002f6433, 0x0046c254, 0xffd7c239, // EQ0, Channel 0, Filter 3
						0x003dfe5c, 0xff8ac187, 0x0038d66e, 0x00753e79, 0xffc92b36, // EQ0, Channel 0, Filter 4
						0x003c1415, 0xff8a7211, 0x003a1f9c, 0x00758def, 0xffc9cc4e, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale 
						0x003e9eb2, 0xff82c29a, 0x003e9eb2, 0x007d34d0, 0xffc2ba05, // EQ0, Channel 1, Filter 0
						0x003c3254, 0xff879b56, 0x003c3254, 0x007823e1, 0xffc75a8f, // EQ0, Channel 1, Filter 1
						0x003bc322, 0xffa5c45a, 0x00333f13, 0x005a3ba6, 0xffd0fdcb, // EQ0, Channel 1, Filter 2
						0x0038d993, 0xffb93dac, 0x002f6433, 0x0046c254, 0xffd7c239, // EQ0, Channel 1, Filter 3
						0x003dfe5c, 0xff8ac187, 0x0038d66e, 0x00753e79, 0xffc92b36, // EQ0, Channel 1, Filter 4
						0x003c1415, 0xff8a7211, 0x003a1f9c, 0x00758def, 0xffc9cc4e, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale 
						0x00400000, 0xff96b842, 0x002cfe7f, 0x006947be, 0xffd30181, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale 
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale 
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOnHp",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0x3a}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd5}, // Compressor Attack Time (High)
						{ 0x2b, 0x92}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low)
						{ 0x30, 0xd5}, // Limiter Attack Time (High)
						{ 0x31, 0x24}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0x3a}, // Expander Attack Time (Low)
						{ 0x36, 0xd5}, // Expander Attack Time (High)
						{ 0x37, 0x92}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x00403494, 0xff80b531, 0x003f179f, 0x007f4acf, 0xffc0b3cc, // EQ0, Channel 0, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ0, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ0, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ0, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale
						0x00403494, 0xff80b531, 0x003f179f, 0x007f4acf, 0xffc0b3cc, // EQ0, Channel 1, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ0, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ0, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ0, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOnHp",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0xb3}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd8}, // Compressor Attack Time (High)
						{ 0x2b, 0x9b}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low)
						{ 0x30, 0xd8}, // Limiter Attack Time (High)
						{ 0x31, 0x36}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0xb3}, // Expander Attack Time (Low)
						{ 0x36, 0xd8}, // Expander Attack Time (High)
						{ 0x37, 0x9b}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x00403054, 0xff80a670, 0x003f2a68, 0x007f5990, 0xffc0a543, // EQ0, Channel 0, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ0, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ0, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ0, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale
						0x00403054, 0xff80a670, 0x003f2a68, 0x007f5990, 0xffc0a543, // EQ0, Channel 1, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ0, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ0, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ0, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffHp",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0x3a}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd5}, // Compressor Attack Time (High)
						{ 0x2b, 0x92}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low)
						{ 0x30, 0xd5}, // Limiter Attack Time (High)
						{ 0x31, 0x24}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0x3a}, // Expander Attack Time (Low)
						{ 0x36, 0xd5}, // Expander Attack Time (High)
						{ 0x37, 0x92}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x003f584c, 0xff81c14b, 0x003ee7ca, 0x007e3eb5, 0xffc1bfe9, // EQ0, Channel 0, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ0, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ0, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ0, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale
						0x003f584c, 0xff81c14b, 0x003ee7ca, 0x007e3eb5, 0xffc1bfe9, // EQ0, Channel 1, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ0, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ0, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ0, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffHp",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0xb3}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd8}, // Compressor Attack Time (High)
						{ 0x2b, 0x9b}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low)
						{ 0x30, 0xd8}, // Limiter Attack Time (High)
						{ 0x31, 0x36}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0xb3}, // Expander Attack Time (Low)
						{ 0x36, 0xd8}, // Expander Attack Time (High)
						{ 0x37, 0x9b}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x003f65c1, 0xff819d25, 0x003efe45, 0x007e62db, 0xffc19bfa, // EQ0, Channel 0, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ0, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ0, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ0, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ0, Channel 0, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 0, Prescale
						0x003f65c1, 0xff819d25, 0x003efe45, 0x007e62db, 0xffc19bfa, // EQ0, Channel 1, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ0, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ0, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ0, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ0, Channel 1, Filter 5
						0x00000000, 0x00232768, // EQ0, Channel 1, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOnHpInEar",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0x3a}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd5}, // Compressor Attack Time (High)
						{ 0x2b, 0x92}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low)
						{ 0x30, 0xd5}, // Limiter Attack Time (High)
						{ 0x31, 0x24}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0x3a}, // Expander Attack Time (Low)
						{ 0x36, 0xd5}, // Expander Attack Time (High)
						{ 0x37, 0x92}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x004046cb, 0xff808fa9, 0x003f2af0, 0x007f7057, 0xffc08e44, // EQ0, Channel 0, Filter 0
						0x004011bc, 0xff80c156, 0x003f3217, 0x007f3eaa, 0xffc0bc2d, // EQ0, Channel 0, Filter 1
						0x003ea4cb, 0xff8b8385, 0x003682be, 0x00747c7b, 0xffcad876, // EQ0, Channel 0, Filter 2
						0x003f541c, 0xff91031b, 0x003452ab, 0x006efce5, 0xffcc5939, // EQ0, Channel 0, Filter 3
						0x004150d8, 0xffa87d92, 0x00291dec, 0x0057826e, 0xffd5913b, // EQ0, Channel 0, Filter 4
						0x00606b26, 0xffa01970, 0x002b92a3, 0x002c47d2, 0xffe7a0f4, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x004046cb, 0xff808fa9, 0x003f2af0, 0x007f7057, 0xffc08e44, // EQ0, Channel 1, Filter 0
						0x004011bc, 0xff80c156, 0x003f3217, 0x007f3eaa, 0xffc0bc2d, // EQ0, Channel 1, Filter 1
						0x003ea4cb, 0xff8b8385, 0x003682be, 0x00747c7b, 0xffcad876, // EQ0, Channel 1, Filter 2
						0x003f541c, 0xff91031b, 0x003452ab, 0x006efce5, 0xffcc5939, // EQ0, Channel 1, Filter 3
						0x004150d8, 0xffa87d92, 0x00291dec, 0x0057826e, 0xffd5913b, // EQ0, Channel 1, Filter 4
						0x00606b26, 0xffa01970, 0x002b92a3, 0x002c47d2, 0xffe7a0f4, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOnHpInEar",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0xb3}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd8}, // Compressor Attack Time (High)
						{ 0x2b, 0x9b}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low)
						{ 0x30, 0xd8}, // Limiter Attack Time (High)
						{ 0x31, 0x36}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0xb3}, // Expander Attack Time (Low)
						{ 0x36, 0xd8}, // Expander Attack Time (High)
						{ 0x37, 0x9b}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x00404110, 0xff8083ee, 0x003f3c2e, 0x007f7c12, 0xffc082c1, // EQ0, Channel 0, Filter 0
						0x0040104d, 0xff80b153, 0x003f42bb, 0x007f4ead, 0xffc0acf7, // EQ0, Channel 0, Filter 1
						0x003ebece, 0xff8a99dd, 0x003738bf, 0x00756623, 0xffca0872, // EQ0, Channel 0, Filter 2
						0x003f60d7, 0xff8f67e4, 0x00353012, 0x0070981c, 0xffcb6f17, // EQ0, Channel 0, Filter 3
						0x004139a8, 0xffa45b3a, 0x002ab12e, 0x005ba4c6, 0xffd41529, // EQ0, Channel 0, Filter 4
						0x0061cd3e, 0xff95f4a3, 0x002ed5af, 0x00333026, 0xffe6384a, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x00404110, 0xff8083ee, 0x003f3c2e, 0x007f7c12, 0xffc082c1, // EQ0, Channel 1, Filter 0
						0x0040104d, 0xff80b153, 0x003f42bb, 0x007f4ead, 0xffc0acf7, // EQ0, Channel 1, Filter 1
						0x003ebece, 0xff8a99dd, 0x003738bf, 0x00756623, 0xffca0872, // EQ0, Channel 1, Filter 2
						0x003f60d7, 0xff8f67e4, 0x00353012, 0x0070981c, 0xffcb6f17, // EQ0, Channel 1, Filter 3
						0x004139a8, 0xffa45b3a, 0x002ab12e, 0x005ba4c6, 0xffd41529, // EQ0, Channel 1, Filter 4
						0x0061cd3e, 0xff95f4a3, 0x002ed5af, 0x00333026, 0xffe6384a, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffHpInEar",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0x3a}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd5}, // Compressor Attack Time (High)
						{ 0x2b, 0x92}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low)
						{ 0x30, 0xd5}, // Limiter Attack Time (High)
						{ 0x31, 0x24}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0x3a}, // Expander Attack Time (Low)
						{ 0x36, 0xd5}, // Expander Attack Time (High)
						{ 0x37, 0x92}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x003f7d8d, 0xff817ed1, 0x003f04e4, 0x007e812f, 0xffc17d8f, // EQ0, Channel 0, Filter 0
						0x00400bd0, 0xff80c6cc, 0x003f328d, 0x007f3934, 0xffc0c1a3, // EQ0, Channel 0, Filter 1
						0x003ea4cb, 0xff8b8385, 0x003682be, 0x00747c7b, 0xffcad876, // EQ0, Channel 0, Filter 2
						0x003f541c, 0xff91031b, 0x003452ab, 0x006efce5, 0xffcc5939, // EQ0, Channel 0, Filter 3
						0x004150d8, 0xffa87d92, 0x00291dec, 0x0057826e, 0xffd5913b, // EQ0, Channel 0, Filter 4
						0x00606b26, 0xffa01970, 0x002b92a3, 0x002c47d2, 0xffe7a0f4, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x003f7d8d, 0xff817ed1, 0x003f04e4, 0x007e812f, 0xffc17d8f, // EQ0, Channel 1, Filter 0
						0x00400bd0, 0xff80c6cc, 0x003f328d, 0x007f3934, 0xffc0c1a3, // EQ0, Channel 1, Filter 1
						0x003ea4cb, 0xff8b8385, 0x003682be, 0x00747c7b, 0xffcad876, // EQ0, Channel 1, Filter 2
						0x003f541c, 0xff91031b, 0x003452ab, 0x006efce5, 0xffcc5939, // EQ0, Channel 1, Filter 3
						0x004150d8, 0xffa87d92, 0x00291dec, 0x0057826e, 0xffd5913b, // EQ0, Channel 1, Filter 4
						0x00606b26, 0xffa01970, 0x002b92a3, 0x002c47d2, 0xffe7a0f4, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffHpInEar",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0xb3}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd8}, // Compressor Attack Time (High)
						{ 0x2b, 0x9b}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low)
						{ 0x30, 0xd8}, // Limiter Attack Time (High)
						{ 0x31, 0x36}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0xb3}, // Expander Attack Time (Low)
						{ 0x36, 0xd8}, // Expander Attack Time (High)
						{ 0x37, 0x9b}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x003f8809, 0xff815ff4, 0x003f1913, 0x007ea00c, 0xffc15ee3, // EQ0, Channel 0, Filter 0
						0x00400adb, 0xff80b659, 0x003f4327, 0x007f49a7, 0xffc0b1fd, // EQ0, Channel 0, Filter 1
						0x003ebece, 0xff8a99dd, 0x003738bf, 0x00756623, 0xffca0872, // EQ0, Channel 0, Filter 2
						0x003f60d7, 0xff8f67e4, 0x00353012, 0x0070981c, 0xffcb6f17, // EQ0, Channel 0, Filter 3
						0x004139a8, 0xffa45b3a, 0x002ab12e, 0x005ba4c6, 0xffd41529, // EQ0, Channel 0, Filter 4
						0x0061cd3e, 0xff95f4a3, 0x002ed5af, 0x00333026, 0xffe6384a, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x003f8809, 0xff815ff4, 0x003f1913, 0x007ea00c, 0xffc15ee3, // EQ0, Channel 1, Filter 0
						0x00400adb, 0xff80b659, 0x003f4327, 0x007f49a7, 0xffc0b1fd, // EQ0, Channel 1, Filter 1
						0x003ebece, 0xff8a99dd, 0x003738bf, 0x00756623, 0xffca0872, // EQ0, Channel 1, Filter 2
						0x003f60d7, 0xff8f67e4, 0x00353012, 0x0070981c, 0xffcb6f17, // EQ0, Channel 1, Filter 3
						0x004139a8, 0xffa45b3a, 0x002ab12e, 0x005ba4c6, 0xffd41529, // EQ0, Channel 1, Filter 4
						0x0061cd3e, 0xff95f4a3, 0x002ed5af, 0x00333026, 0xffe6384a, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOnHpPassive",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0x3a}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd5}, // Compressor Attack Time (High)
						{ 0x2b, 0x92}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low)
						{ 0x30, 0xd5}, // Limiter Attack Time (High)
						{ 0x31, 0x24}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0x3a}, // Expander Attack Time (Low)
						{ 0x36, 0xd5}, // Expander Attack Time (High)
						{ 0x37, 0x92}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x00403494, 0xff80b531, 0x003f179f, 0x007f4acf, 0xffc0b3cc, // EQ0, Channel 0, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 0, Filter 1
						0x004005e5, 0xff802ddc, 0x003fcc90, 0x007fd224, 0xffc02d8b, // EQ0, Channel 0, Filter 2
						0x0040093f, 0xff806385, 0x003f94a1, 0x007f9c7b, 0xffc0621f, // EQ0, Channel 0, Filter 3
						0x004535c6, 0xfff33ad2, 0x00128bd6, 0x000cc52e, 0xffe83e64, // EQ0, Channel 0, Filter 4
						0x004410b3, 0x003b5bf6, 0x001c379e, 0xffc0d800, 0xffe383b9, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x00403494, 0xff80b531, 0x003f179f, 0x007f4acf, 0xffc0b3cc, // EQ0, Channel 1, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 1, Filter 1
						0x004005e5, 0xff802ddc, 0x003fcc90, 0x007fd224, 0xffc02d8b, // EQ0, Channel 1, Filter 2
						0x0040093f, 0xff806385, 0x003f94a1, 0x007f9c7b, 0xffc0621f, // EQ0, Channel 1, Filter 3
						0x004535c6, 0xfff33ad2, 0x00128bd6, 0x000cc52e, 0xffe83e64, // EQ0, Channel 1, Filter 4
						0x004410b3, 0x003b5bf6, 0x001c379e, 0xffc0d800, 0xffe383b9, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOnHpPassive",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0xb3}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd8}, // Compressor Attack Time (High)
						{ 0x2b, 0x9b}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low)
						{ 0x30, 0xd8}, // Limiter Attack Time (High)
						{ 0x31, 0x36}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0xb3}, // Expander Attack Time (Low)
						{ 0x36, 0xd8}, // Expander Attack Time (High)
						{ 0x37, 0x9b}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x00403054, 0xff80a670, 0x003f2a68, 0x007f5990, 0xffc0a543, // EQ0, Channel 0, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 0, Filter 1
						0x0040056a, 0xff802a1d, 0x003fd0bc, 0x007fd5e3, 0xffc029d9, // EQ0, Channel 0, Filter 2
						0x0040087f, 0xff805b5a, 0x003f9d54, 0x007fa4a6, 0xffc05a2c, // EQ0, Channel 0, Filter 3
						0x0044e564, 0xffe8a8c6, 0x00154919, 0x0017573a, 0xffe5d182, // EQ0, Channel 0, Filter 4
						0x0045067a, 0x002ad8bc, 0x00190503, 0xffcfe36c, 0xffe7385b, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x00403054, 0xff80a670, 0x003f2a68, 0x007f5990, 0xffc0a543, // EQ0, Channel 1, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 1, Filter 1
						0x0040056a, 0xff802a1d, 0x003fd0bc, 0x007fd5e3, 0xffc029d9, // EQ0, Channel 1, Filter 2
						0x0040087f, 0xff805b5a, 0x003f9d54, 0x007fa4a6, 0xffc05a2c, // EQ0, Channel 1, Filter 3
						0x0044e564, 0xffe8a8c6, 0x00154919, 0x0017573a, 0xffe5d182, // EQ0, Channel 1, Filter 4
						0x0045067a, 0x002ad8bc, 0x00190503, 0xffcfe36c, 0xffe7385b, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffHpPassive",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0x3a}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd5}, // Compressor Attack Time (High)
						{ 0x2b, 0x92}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0x3a}, // Limiter Attack Time (Low)
						{ 0x30, 0xd5}, // Limiter Attack Time (High)
						{ 0x31, 0x24}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0x3a}, // Expander Attack Time (Low)
						{ 0x36, 0xd5}, // Expander Attack Time (High)
						{ 0x37, 0x92}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 44100,
				.dsp_params = {
						0x003f584c, 0xff81c14b, 0x003ee7ca, 0x007e3eb5, 0xffc1bfe9, // EQ0, Channel 0, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 0, Filter 1
						0x004005e5, 0xff802ddc, 0x003fcc90, 0x007fd224, 0xffc02d8b, // EQ0, Channel 0, Filter 2
						0x0040093f, 0xff806385, 0x003f94a1, 0x007f9c7b, 0xffc0621f, // EQ0, Channel 0, Filter 3
						0x004535c6, 0xfff33ad2, 0x00128bd6, 0x000cc52e, 0xffe83e64, // EQ0, Channel 0, Filter 4
						0x004410b3, 0x003b5bf6, 0x001c379e, 0xffc0d800, 0xffe383b9, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x003f584c, 0xff81c14b, 0x003ee7ca, 0x007e3eb5, 0xffc1bfe9, // EQ0, Channel 1, Filter 0
						0x0054e356, 0x000bcd4f, 0xffe3afd3, 0xfff432b1, 0x00076cd6, // EQ0, Channel 1, Filter 1
						0x004005e5, 0xff802ddc, 0x003fcc90, 0x007fd224, 0xffc02d8b, // EQ0, Channel 1, Filter 2
						0x0040093f, 0xff806385, 0x003f94a1, 0x007f9c7b, 0xffc0621f, // EQ0, Channel 1, Filter 3
						0x004535c6, 0xfff33ad2, 0x00128bd6, 0x000cc52e, 0xffe83e64, // EQ0, Channel 1, Filter 4
						0x004410b3, 0x003b5bf6, 0x001c379e, 0xffc0d800, 0xffe383b9, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff875fc1, 0x0039dc57, 0x0078a03f, 0xffc623a9, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff9068f1, 0x0034476c, 0x006f970f, 0xffcbb894, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff9ad300, 0x002f2d89, 0x00652d00, 0xffd0d277, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa65674, 0x002a7e43, 0x0059a98c, 0xffd581bd, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffb2ad1f, 0x00262bb6, 0x004d52e1, 0xffd9d44a, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffbf9287, 0x002229ed, 0x00406d79, 0xffddd613, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT1
						0x0000c254, 0x00000000, 0xffff3dab, 0x007e5dc5, 0xffc184a9, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000013d6, 0x000027ac, 0x000013d6, 0x0079e632, 0xffc5ca75, // Bass_Coef_LMT
						0x003d9ccf, 0xff84c660, 0x003d9ccf, 0x007b1fff, 0xffc4acc0, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT1
						0x0025ffd4, 0xffb40057, 0x0025ffd4, 0x00417a75, 0xffe97b24, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x0009a184, 0x00134309, 0x0009a184, 0xffee6f53, 0x002b0a9a, // Treb_Coef_LMT
						0x00400000, 0xff84e001, 0x003b5340, 0x007b1fff, 0xffc4acc0, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
		{
				.name = "BeatsOffHpPassive",
				.preset_regs = {
						{ 0x20, 0x6e}, // Config 1
						{ 0x25, 0x00}, // Comp/Lim/Exp (CLE) Control
						{ 0x26, 0x00}, // CLE Make-up Gain
						{ 0x27, 0xc9}, // Compressor Threshold
						{ 0x28, 0x0a}, // Compressor Ratio
						{ 0x29, 0xb3}, // Compressor Attack Time (Low)
						{ 0x2a, 0xd8}, // Compressor Attack Time (High)
						{ 0x2b, 0x9b}, // Compressor Release Time (Low)
						{ 0x2c, 0xff}, // Compressor Release Time (High)
						{ 0x2d, 0xf1}, // Limiter Threshold
						{ 0x2e, 0xf1}, // Limiter Target
						{ 0x2f, 0xb3}, // Limiter Attack Time (Low)
						{ 0x30, 0xd8}, // Limiter Attack Time (High)
						{ 0x31, 0x36}, // Limiter Release Time (Low)
						{ 0x32, 0xff}, // Limiter Release Time (High)
						{ 0x33, 0x94}, // Expander Threshold
						{ 0x34, 0x01}, // Expander Ratio
						{ 0x35, 0xb3}, // Expander Attack Time (Low)
						{ 0x36, 0xd8}, // Expander Attack Time (High)
						{ 0x37, 0x9b}, // Expander Release Time (Low)
						{ 0x38, 0xff}, // Expander Release Time (High)
						{ 0x39, 0x00}, // Effects Control
						{ -1, -1 }
				},
				.sample_rate = 48000,
				.dsp_params = {
						0x003f65c1, 0xff819d25, 0x003efe45, 0x007e62db, 0xffc19bfa, // EQ0, Channel 0, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 0, Filter 1
						0x0040056a, 0xff802a1d, 0x003fd0bc, 0x007fd5e3, 0xffc029d9, // EQ0, Channel 0, Filter 2
						0x0040087f, 0xff805b5a, 0x003f9d54, 0x007fa4a6, 0xffc05a2c, // EQ0, Channel 0, Filter 3
						0x0044e564, 0xffe8a8c6, 0x00154919, 0x0017573a, 0xffe5d182, // EQ0, Channel 0, Filter 4
						0x0045067a, 0x002ad8bc, 0x00190503, 0xffcfe36c, 0xffe7385b, // EQ0, Channel 0, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 0, Prescale
						0x003f65c1, 0xff819d25, 0x003efe45, 0x007e62db, 0xffc19bfa, // EQ0, Channel 1, Filter 0
						0x0053ac76, 0x0003f8c4, 0xffe90db6, 0xfffc073c, 0x000345d4, // EQ0, Channel 1, Filter 1
						0x0040056a, 0xff802a1d, 0x003fd0bc, 0x007fd5e3, 0xffc029d9, // EQ0, Channel 1, Filter 2
						0x0040087f, 0xff805b5a, 0x003f9d54, 0x007fa4a6, 0xffc05a2c, // EQ0, Channel 1, Filter 3
						0x0044e564, 0xffe8a8c6, 0x00154919, 0x0017573a, 0xffe5d182, // EQ0, Channel 1, Filter 4
						0x0045067a, 0x002ad8bc, 0x00190503, 0xffcfe36c, 0xffe7385b, // EQ0, Channel 1, Filter 5
						0x00000000, 0x001948ed, // EQ0, Channel 1, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 0, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 0, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 0, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 0, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 0, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 0, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 0, Prescale
						0x00400000, 0xff86b584, 0x003a566a, 0x00794a7c, 0xffc5a996, // EQ1, Channel 1, Filter 0
						0x00400000, 0xff8ed722, 0x003526c6, 0x007128de, 0xffcad93a, // EQ1, Channel 1, Filter 1
						0x00400000, 0xff982c80, 0x00306170, 0x0067d380, 0xffcf9e90, // EQ1, Channel 1, Filter 2
						0x00400000, 0xffa27dfe, 0x002bf93a, 0x005d8202, 0xffd406c6, // EQ1, Channel 1, Filter 3
						0x00400000, 0xffad94df, 0x0027e2de, 0x00526b21, 0xffd81d22, // EQ1, Channel 1, Filter 4
						0x00400000, 0xffb93b7f, 0x00241499, 0x0046c481, 0xffdbeb67, // EQ1, Channel 1, Filter 5
						0x00000000, 0x00400000, // EQ1, Channel 1, Prescale
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT1
						0x0000b2b6, 0x00000000, 0xffff4d49, 0x007e8197, 0xffc1656d, // Bass_Coef_EXT2
						0x00000000, 0x00000000, // Bass_Coef_NLF
						0x000010ce, 0x0000219d, 0x000010ce, 0x007a65c3, 0xffc55703, // Bass_Coef_LMT
						0x003dcdc3, 0xff846478, 0x003dcdc3, 0x007b85d5, 0xffc44ec7, // Bass_Coef_CTO
						0x00000000, // Bass_MIX
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT1
						0x0027dcfd, 0xffb04605, 0x0027dcfd, 0x00468a27, 0xffe71633, // Treb_Coef_EXT2
						0x00000000, 0x00000000, // Treb_Coef_NLF
						0x000e718b, 0x001ce317, 0x000e718b, 0xffe811b5, 0x001e281d, // Treb_Coef_LMT
						0x00400000, 0xff847a2b, 0x003bb139, 0x007b85d5, 0xffc44ec7, // Treb_Coef_CTO
						0x00000000, // Treb_MIX
						0x00000000, // 3D_Coef
						0x00400000, // 3D_Mix
					}
		},
};
