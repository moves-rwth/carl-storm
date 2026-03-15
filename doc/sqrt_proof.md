# Square root proof

Let $\frac{n}{d} > 0$ be a rational number.
We want to compute $\sqrt{\frac{n}{d}}$.
The current algorithm will return an interval
$[\frac{\lfloor{\sqrt{n}\rfloor}}{\lfloor\sqrt{d}\rfloor+1}, \frac{\lfloor\sqrt{n}\rfloor+1}{\lfloor\sqrt{d}\rfloor}]$ in which the square root lies.

We now want to achieve a given relative precision $\varepsilon > 0$ such that
$$\frac{\lfloor\sqrt{n}\rfloor+1}{\lfloor\sqrt{d}\rfloor}-\frac{\lfloor\sqrt{n}\rfloor}{\lfloor\sqrt{d}\rfloor+1}\leq\varepsilon\frac{\lfloor\sqrt{n}\rfloor+1}{\lfloor\sqrt{d}\rfloor}$$

To this end, we introduce a constant $x$ and multiply both $n$ and $d$ with this constant.
As $\frac{nx}{dx}=\frac{n}{d}$ this does not change the result.
TODO: still changes result!!!

But we can influence the precision by choosing $x$ such that
$$\frac{\lfloor\sqrt{nx}\rfloor+1}{\lfloor\sqrt{dx}\rfloor}-\frac{\lfloor\sqrt{nx}\rfloor}{\lfloor\sqrt{dx}\rfloor+1}\leq\varepsilon\frac{\lfloor\sqrt{nx}\rfloor+1}{\lfloor\sqrt{dx}\rfloor}$$


We propose the following lower bound on $x$ which achieves the desired precision:
$$ x \geq \frac{2}{\varepsilon^2} \left(\frac{1}{d} + \frac{1}{n}\right)$$


## Proof
We need to ensure:
$$
\begin{align}
&&\frac{\lfloor\sqrt{nx}\rfloor+1}{\lfloor\sqrt{dx}\rfloor}-\frac{\lfloor\sqrt{nx}\rfloor}{\lfloor\sqrt{dx}\rfloor+1} &\leq\varepsilon\frac{\lfloor\sqrt{nx}\rfloor+1}{\lfloor\sqrt{dx}\rfloor} \\
\overset{\text{multiply }\lfloor\sqrt{dx}\rfloor(\lfloor\sqrt{dx}\rfloor+1)}\iff &&(\lfloor\sqrt{nx}\rfloor+1)(\lfloor\sqrt{dx}\rfloor+1)-\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor &\leq\varepsilon (\lfloor\sqrt{nx}\rfloor+1)(\lfloor\sqrt{dx}\rfloor+1) \\
\iff && (\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor+\lfloor\sqrt{nx}\rfloor+\lfloor\sqrt{dx}\rfloor+1)-\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor & \leq\varepsilon (\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor+\lfloor\sqrt{nx}\rfloor+\lfloor\sqrt{dx}\rfloor+1)\\
\iff && \lfloor\sqrt{nx}\rfloor+\lfloor\sqrt{dx}\rfloor+1 - \varepsilon (\lfloor\sqrt{nx}\rfloor+\lfloor\sqrt{dx}\rfloor+1) &\leq \varepsilon \lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor\\
\iff && (1-\varepsilon)(\lfloor\sqrt{nx}\rfloor+\lfloor\sqrt{dx}\rfloor+1) &\leq \varepsilon \lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor \\
\iff && \lfloor\sqrt{nx}\rfloor+\lfloor\sqrt{dx}\rfloor+1 &\leq \frac{\varepsilon}{(1-\varepsilon)} \lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor \\
\iff && \frac{\lfloor\sqrt{nx}\rfloor}{\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor}+\frac{\lfloor\sqrt{dx}\rfloor}{\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor}+\frac{1}{\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor} &\leq \frac{\varepsilon}{(1-\varepsilon)} \\
\iff && \frac{1}{\lfloor\sqrt{dx}\rfloor}+\frac{1}{\lfloor\sqrt{nx}\rfloor}+\frac{1}{\lfloor\sqrt{nx}\rfloor\lfloor\sqrt{dx}\rfloor} &\leq \frac{\varepsilon}{(1-\varepsilon)} \\
\overset{\sqrt{nx}=\sqrt{\frac{n}{d}}\sqrt{dx}}\iff && \frac{1}{\lfloor\sqrt{dx}\rfloor}+\frac{1}{\lfloor\sqrt{\frac{n}{d}}\sqrt{dx}\rfloor}+\frac{1}{\lfloor\sqrt{\frac{n}{d}}\sqrt{dx}\rfloor\lfloor\sqrt{dx}\rfloor} &\leq \frac{\varepsilon}{(1-\varepsilon)} \\
\end{align}
$$

We use the following inequality:
$$
\frac{1}{\lfloor\sqrt{nx}\rfloor}
= \frac{1}{\lfloor\sqrt{\frac{n}{d}}\sqrt{dx}\rfloor}
\leq \frac{1}{\lfloor\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor\rfloor}
\leq \frac{1}{\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor - 1}
$$
and obtain a more conservative bound for the left-hand side:
$$
\begin{align}
& \frac{1}{\lfloor\sqrt{dx}\rfloor}+\frac{1}{\lfloor\sqrt{\frac{n}{d}}\sqrt{dx}\rfloor}+\frac{1}{\lfloor\sqrt{\frac{n}{d}}\sqrt{dx}\rfloor\lfloor\sqrt{dx}\rfloor} \\
\leq & \frac{1}{\lfloor\sqrt{dx}\rfloor}+\frac{1}{\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1}+\frac{1}{(\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1)\lfloor\sqrt{dx}\rfloor} \\
\end{align}
$$


We are therefore interested in the inequality
$$
\begin{align}
&& \frac{1}{\lfloor\sqrt{dx}\rfloor}+\frac{1}{\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1}+\frac{1}{(\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1)\lfloor\sqrt{dx}\rfloor} &\leq \frac{\varepsilon}{(1-\varepsilon)} \\
\iff && \sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1 + \lfloor\sqrt{dx}\rfloor + 1 &\leq \frac{\varepsilon}{(1-\varepsilon)} \left(\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1\right)\lfloor\sqrt{dx}\rfloor\\
\iff && 0 &\leq \frac{\varepsilon}{(1-\varepsilon)} \left(\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1\right)\lfloor\sqrt{dx}\rfloor - \sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor - \lfloor\sqrt{dx}\rfloor \\
\iff && 0 &\leq \lfloor\sqrt{dx}\rfloor \left(\frac{\varepsilon}{(1-\varepsilon)} \left(\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1\right) - \sqrt{\frac{n}{d}} - 1 \right) \\
\overset{\lfloor\sqrt{dx}\rfloor\geq0}\implies && 0 &\leq \frac{\varepsilon}{(1-\varepsilon)} \left(\sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor-1\right) - \sqrt{\frac{n}{d}} - 1\\
\iff && \sqrt{\frac{n}{d}} + 1 + \frac{\varepsilon}{(1-\varepsilon)} &\leq \frac{\varepsilon}{(1-\varepsilon)} \sqrt{\frac{n}{d}}\lfloor\sqrt{dx}\rfloor \\
\iff && \frac{1-\varepsilon}{\varepsilon} + \frac{(1-\varepsilon)\sqrt{\frac{d}{n}}}{\varepsilon} + \sqrt{\frac{d}{n}} &\leq \lfloor\sqrt{dx}\rfloor \\
\implies && \left\lceil\frac{1-\varepsilon}{\varepsilon} + \frac{(1-\varepsilon)\sqrt{\frac{d}{n}}}{\varepsilon} + \sqrt{\frac{d}{n}}\right\rceil &\leq \sqrt{dx} \\
\iff && \left\lceil\frac{1-\varepsilon}{\varepsilon} + \frac{(1-\varepsilon)\sqrt{\frac{d}{n}}}{\varepsilon} + \sqrt{\frac{d}{n}}\right\rceil^2 \frac{1}{d} &\leq x \\
\iff && \left\lceil\frac{1-\varepsilon}{\varepsilon} + \sqrt{\frac{d}{n}}(\frac{1}{\varepsilon} -1+1)\right\rceil^2 \frac{1}{d} &\leq x \\
\iff && \left\lceil\frac{1}{\varepsilon}\left(1+\sqrt{\frac{d}{n}}\right)-1\right\rceil^2 \frac{1}{d} &\leq x \\
\iff && \left\lceil\frac{1}{\varepsilon}\left(\frac{\sqrt{n}+\sqrt{d}}{\sqrt{n}}\right)-1\right\rceil^2 \frac{1}{d} &\leq x \\
\end{align}
$$
We can again obtain a more conservative bound:
$$
\begin{align}
 &\left\lceil\frac{1}{\varepsilon}\left(\frac{\sqrt{n}+\sqrt{d}}{\sqrt{n}}\right)-1\right\rceil^2 \frac{1}{d} \\
\leq &\left(\frac{1}{\varepsilon}\left(\frac{\sqrt{n}+\sqrt{d}}{\sqrt{n}}\right)-1+1\right)^2 \frac{1}{d} \\
= &\frac{1}{d\varepsilon^2}\left(\frac{\sqrt{n}+\sqrt{d}}{\sqrt{n}}\right)^2 \\
= &\frac{1}{dn\varepsilon^2}\left(n+2\sqrt{nd}+d\right)\\
\end{align}
$$

using the the AM–GM inequality $\sqrt{uv} \leq \frac{u+v}{2}$ (for $u,v > 0$).
$$
\begin{align}
&\frac{1}{dn\varepsilon^2}\left(n+2\sqrt{nd}+d\right) \\
\leq &\frac{1}{dn\varepsilon^2}\left(n+(n+d)+d\right) \\
= &\frac{1}{\varepsilon^2} \left(\frac{2}{d} + \frac{2}{n}\right) \\
\end{align}
$$
This is the final bound
$$ x \geq \frac{2}{\varepsilon^2} \left(\frac{1}{d} + \frac{1}{n}\right)$$