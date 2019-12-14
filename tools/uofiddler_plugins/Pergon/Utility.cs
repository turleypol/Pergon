using System;

namespace Pergon
{
    public class Utility
    {
        private static Random m_Random = new Random();

        public static double RandomDouble()
        {
            return m_Random.NextDouble();
        }

        public static bool RandomBool()
        {
            return (m_Random.Next(2) == 0);
        }

        public static int Random(int count)
        {
            return m_Random.Next(count);
        }
    }
}