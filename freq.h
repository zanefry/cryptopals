char* eng_chars = "abcdefghijklmnopqrstuvwxyz ";
size_t eng_chars_length = 27;
float eng_freqs[27] = { 0.0651738, 0.0124248, 0.0217339, 0.0349835, 0.1041442, 0.0197881, 0.0158610, 0.0492888, 0.0558094, 0.0009033, 0.0050529, 0.0331490, 0.0202124, 0.0564513, 0.0596302, 0.0137645, 0.0008606, 0.0497563, 0.0515760, 0.0729357, 0.0225134, 0.0082903, 0.0171272, 0.0013692, 0.0145984, 0.0007836, 0.1918182 };

float rank (bytestring string)
{
  float rank;

  for (int i = 0; i < string.length; i++)
    for (int j = 0; j < eng_chars_length; j++)
      if (string.buf[i] == eng_chars[j])
	rank += eng_freqs[j];

  return rank;
}
