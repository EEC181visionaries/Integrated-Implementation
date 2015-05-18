% cFormat(array_name, file_name)
% turns matrixies to c format
% Ex. a = [1 2; 3 4]
% -> {1,2},
%    {3;4}

function cFormat(array_name, file_name)

[rows, cols] = size(array_name);
FILE = fopen(file_name, 'w');

for i = 1:rows
    fprintf(FILE, '{');
    for (j = 1:cols)
        fprintf(FILE, '%d', array_name(i,j));
        if j ~= cols
            fprintf(FILE, ',');
        end
    end
    
    if (i ~= rows)
        fprintf(FILE, '},\n');
    else
        fprintf(FILE, '}\n');
    end
end

fclose(FILE);
