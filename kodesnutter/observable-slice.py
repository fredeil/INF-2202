___author___ = "Mads Johansen"


class Observable:

    def __getitem__(self, obj):
    source = self
    start = obj.start or 0
    stop = obj.stop or 0
    step = obj.step or 1

    if step < 0:
        raise ArgumentError("Cannot slice an Observable with negative step value")

    sliced = source.skip(star) if start >= 0 else source.take_last(abs(start))

    sliced = sliced.take(stop - start) if stop >= 0 else sliced.skip_last(abs(stop))

    sliced = sliced.skip(step - 1)

    return sliced

    def __getitem__(self, obj):
        easy_solution = self.toList().__getitem__(obj)
        return Observable._from(easy_solution)
