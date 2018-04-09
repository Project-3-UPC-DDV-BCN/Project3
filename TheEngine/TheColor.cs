using System;

namespace TheEngine
{
    public class TheColor
    {
        public float r;
        public float g;
        public float b;
        public float a;

        public TheColor(float r, float g, float b, float a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        public float this[int index]
        {
            get
            {
                float result;
                switch (index)
                {
                    case 0:
                        result = r;
                        break;
                    case 1:
                        result = g;
                        break;
                    case 2:
                        result = b;
                        break;
                    case 3:
                        result = a;
                        break;
                    default:
                        throw new IndexOutOfRangeException("Invalid Color index!");
                }
                return result;
            }
            set
            {
                switch (index)
                {
                    case 0:
                        r = value;
                        break;
                    case 1:
                        g = value;
                        break;
                    case 2:
                        b = value;
                        break;
                    case 3:
                        a = value;
                        break;
                    default:
                        throw new IndexOutOfRangeException("Invalid Color index!");
                }
            }
        }

        public static TheColor Red
        {
            get
            {
                return new TheColor(1f, 0f, 0f, 1f);
            }
        }

        public static TheColor Green
        {
            get
            {
                return new TheColor(0f, 1f, 0f, 1f);
            }
        }

        public static TheColor Blue
        {
            get
            {
                return new TheColor(0f, 0f, 1f, 1f);
            }
        }

        public static TheColor White
        {
            get
            {
                return new TheColor(1f, 1f, 1f, 1f);
            }
        }

        public static TheColor Black
        {
            get
            {
                return new TheColor(0f, 0f, 0f, 1f);
            }
        }

        public static TheColor Yellow
        {
            get
            {
                return new TheColor(1f, 0.921568632f, 0.0156862754f, 1f);
            }
        }

        public static TheColor Cyan
        {
            get
            {
                return new TheColor(0f, 1f, 1f, 1f);
            }
        }

        public static TheColor Magenta
        {
            get
            {
                return new TheColor(1f, 0f, 1f, 1f);
            }
        }

        public static TheColor Grey
        {
            get
            {
                return new TheColor(0.5f, 0.5f, 0.5f, 1f);
            }
        }

        public static TheColor Transparent
        {
            get
            {
                return new TheColor(0f, 0f, 0f, 0f);
            }
        }

        public float Grayscale
        {
            get
            {
                return 0.299f * r + 0.587f * g + 0.114f * b;
            }
        }

        public static TheColor Lerp(TheColor a, TheColor b, float t)
        {
            t = TheMath.TheMath.Clamp(t, 0, 1);
            return new TheColor(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
        }

        public static TheColor LerpUnclamped(TheColor a, TheColor b, float t)
        {
            return new TheColor(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
        }

        public static TheColor operator +(TheColor a, TheColor b)
        {
            return new TheColor(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
        }

        public static TheColor operator -(TheColor a, TheColor b)
        {
            return new TheColor(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
        }

        public static TheColor operator *(TheColor a, TheColor b)
        {
            return new TheColor(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
        }

        public static TheColor operator *(TheColor a, float b)
        {
            return new TheColor(a.r * b, a.g * b, a.b * b, a.a * b);
        }

        public static TheColor operator *(float b, TheColor a)
        {
            return new TheColor(a.r * b, a.g * b, a.b * b, a.a * b);
        }

        public static TheColor operator /(TheColor a, float b)
        {
            return new TheColor(a.r / b, a.g / b, a.b / b, a.a / b);
        }

        public static bool operator ==(TheColor lhs, TheColor rhs)
        {
            return lhs == rhs;
        }

        public static bool operator !=(TheColor lhs, TheColor rhs)
        {
            return !(lhs == rhs);
        }

        public override string ToString()
        {
            return string.Format("RGBA({0:F3}, {1:F3}, {2:F3}, {3:F3})", new object[]
            {
                r,
                g,
                b,
                a
            });
        }

        public override bool Equals(object other)
        {
            bool result;
            if (!(other is TheColor))
            {
                result = false;
            }
            else
            {
                TheColor color = (TheColor)other;
                result = (this.r.Equals(color.r) && g.Equals(color.g) && b.Equals(color.b) && a.Equals(color.a));
            }
            return result;
        }
    }
}
